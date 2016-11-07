/** 
 * @Copyright 2010 GanJi Inc.
 * @file    src/ganji/util/db/db_pool.cc
 * @namespace ganji::util::db
 * @version 1.0
 * @author  huanghao
 * @date    2010-07-27
 * 
 * an implement of mysql database pool 
 *
 * Change Log:
 *
 */

#include <string.h>
#include <assert.h>
#include <mysql/errmsg.h>

#include <string>

#include "util/text/text.h"
#include "db_pool.h"

using std::string;
using std::vector;
using ganji::util::thread::Mutex;
using ganji::util::thread::MutexGuard;
namespace Text = ganji::util::text::Text;

namespace ganji { namespace util { namespace db {

DBPool::DBPool(uint32_t s) : db_pool_size_(s) {
}

DBPool::~DBPool() {
  while (!db_connection_stack_.empty()) {
    DBConnectNode node = db_connection_stack_.top();
    db_connection_stack_.pop();
    if (node.is_connected_) {
      mysql_close(node.db_);
      delete node.db_;
      node.is_connected_ = false;
    }
  }
}

bool DBPool::EscapeString(const string &input, string *output) {
  if (input.length() == 0) {
    return false;
  }
  char * buf = new char[input.length()*2+1];
  uint32_t nLen = mysql_escape_string(buf, input.c_str(), input.length());
  buf[nLen]='\0';
  *output = buf;
  delete [] buf;
  return true;
}

bool DBPool::Initialize(const string &h, const string &port, const string &u, const string &pwd, const string &n) {
  if (!db_connection_stack_.empty()) {
    return false;
  }

  if (0 == db_pool_size_) {
    return false;
  }

  if (db_pool_size_ > 100) {
    db_pool_size_ = 100;
  }

  this->db_host_ = h;
  this->db_port_ = port;
  this->db_usr_ = u;
  this->db_pwd_ = pwd;
  this->db_name_ = n;

  db_port_int_ = (uint32_t)Text::StrToInt(db_port_);
  if ((db_port_int_ < 1) || (db_port_int_ > 65535)) {
    return false;
  }

  for (uint32_t i = 0; i < db_pool_size_; ++i) {
    DBConnectNode node;
    node.db_ = new MYSQL;
    if (!node.db_) {
      return false;
    }
    mysql_init(node.db_);
    if (!mysql_real_connect(node.db_, db_host_.c_str(), db_usr_.c_str(),
                            db_pwd_.c_str(), db_name_.c_str(), db_port_int_, NULL, 0)) {
      mysql_close(node.db_);
      delete node.db_;
      return false;
    }
    mysql_query(node.db_, "set names 'utf8'");
    node.is_connected_ = true;
    db_connection_stack_.push(node);
  }
  return true;
}

int32_t DBPool::Execute(const string &strsql, SqlResult *presult) {
  DBConnectNode node;
  SqlResult &result = *presult;
  bool  bret = AcquireConnection(&node);
  if (!bret) {
    return -2;
  }
  bret = false;
  int iret = 0;
  do {
    if (!node.is_connected_) {
      break;
    }
    mysql_autocommit(node.db_, 1);
    iret = mysql_query(node.db_, strsql.c_str());
    if (iret != 0) {
      iret = mysql_errno(node.db_);
      result.error_no = iret;
    } else {
        result.affected_rows = mysql_affected_rows(node.db_);
        result.insert_id = mysql_insert_id(node.db_);
    }
    if ((CR_ERROR_FIRST <= iret) && (iret <= CR_ERROR_LAST)) {
      mysql_close(node.db_);
      delete node.db_;
      node.db_ = 0;
      node.is_connected_ = false;
      break;
    }
    if (iret != 0) {
      break;
    }
    bret = true;
    result.error_no = 0;
  } while (false);

  ReleaseConnection(node);
  return ( bret ? 0 : -1 );
}

int32_t DBPool::Execute(const string & strsql) {
  DBConnectNode node;
  bool  bret =AcquireConnection(&node);
  if (!bret)
    return -2;
  bret = false;
  int iret = 0;
  do {
    if (!node.is_connected_)
      break;
    mysql_autocommit(node.db_, 1);
    iret = mysql_query(node.db_, strsql.c_str());
    if (iret != 0)
      iret = mysql_errno(node.db_);
    if ((CR_ERROR_FIRST <= iret) && (iret <= CR_ERROR_LAST)) {
      mysql_close(node.db_);
      delete node.db_;
      node.db_ = 0;
      node.is_connected_ = false;
      break;
    }
    if (iret != 0)
      break;
    bret = true;
  } while (false);

  ReleaseConnection(node);
  return ( bret ? 0 : -1 );
}

int32_t DBPool::ExecuteBatch(const std::vector<string> &vecsql) {
  if (vecsql.empty()) {
    return 0;
  }
  DBConnectNode node;
  bool  bret = AcquireConnection(&node);
  if (!bret) {
    return -2;
  }
  bret = false;
  int iret = 0;
  do {
    bret = false;
    mysql_autocommit(node.db_, 0);
    bool isDBDisconnected = false;
    std::vector<string>::const_iterator it = vecsql.begin();
    for ( ; it != vecsql.end(); ++it) {
      iret = mysql_query(node.db_, (*it).c_str());
      if (iret != 0) {
        iret = mysql_errno(node.db_);
      }
      if ((CR_ERROR_FIRST <= iret) && (iret <= CR_ERROR_LAST)) {
        isDBDisconnected = true;
        break;
      }
      if (iret != 0) {
        break;
      }
    }

    if (isDBDisconnected) {
      mysql_close(node.db_);
      delete node.db_;
      node.db_ = 0;
      node.is_connected_ = false;
      break;
    }

    if (iret != 0) {
      mysql_rollback(node.db_);
      mysql_autocommit(node.db_, 1);
      break;
    } else {
      mysql_commit(node.db_);
      mysql_autocommit(node.db_, 1);
    }
    bret = true;
  } while (false);

  ReleaseConnection(node);
  return (bret ? 0 : -1);
}

int32_t DBPool::Query(const string &strsql, SqlResult *presult) {
  DBConnectNode node;
  SqlResult &result = *presult;
  bool  bret = AcquireConnection(&node);
  if (!bret) {
    return -2;
  }
  bret = false;
  int iret = 0;
  do {
    bret = false;
    if (!node.is_connected_) {
      break;
    }
    mysql_autocommit(node.db_, 1);
    iret = mysql_query(node.db_, strsql.c_str());
    if (iret != 0) {
      iret = mysql_errno(node.db_);
    }
    if ((CR_ERROR_FIRST <= iret) && (iret <= CR_ERROR_LAST)) {
      mysql_close(node.db_);
      delete node.db_;
      node.db_ = 0;
      node.is_connected_ = false;
      break;
    }
    if (iret != 0) {
      break;
    }

    MYSQL_RES * dbresult = mysql_store_result(node.db_);
    if (!dbresult) {
      break;
    }

    MYSQL_ROW row;
    int num_field = mysql_num_fields(dbresult);
    MYSQL_FIELD *fields = mysql_fetch_fields(dbresult);
    for (int i = 0; i < num_field; i++)
      result.vec_field_name.push_back(fields[i].name);
    int num_row = mysql_num_rows(dbresult);
    vector<vector<string> > &vec_record = result.vec_record;
    vec_record.resize(num_row);
    for (int i = 0; i < num_row; i++) {
      vector<string> &vecField = vec_record[i];
      vecField.resize(num_field);
      if ((row = mysql_fetch_row(dbresult)) != NULL) {
        for (int j = 0; j < num_field; j++) {
          vecField[j] = (NULL ==row[j]) ? string("") : string(row[j]);
        }
      }
    }
    mysql_free_result(dbresult);
    bret = true;
    presult = &result;
  } while (false);

  ReleaseConnection(node);
  return (bret ? 0 : -1);
}

int32_t DBPool::Query(const string &strsql, string *result_pstr) {
  string &strresult = *result_pstr;
  strresult.resize(0);
  DBConnectNode node;
  bool  bret = AcquireConnection(&node);
  if(!bret)
    return -2;
  bret = false;
  int iret = 0;
  do {
    bret = false;
    if (!node.is_connected_)
      break;
    mysql_autocommit(node.db_, 1);
    iret = mysql_query(node.db_, strsql.c_str());
    if (iret != 0)
      iret = mysql_errno(node.db_);
    if ((CR_ERROR_FIRST <= iret) && (iret <= CR_ERROR_LAST)) {
      mysql_close(node.db_);
      delete node.db_;
      node.db_ = 0;
      node.is_connected_= false;
      break;
    }
    if (iret != 0)
      break;

    MYSQL_RES * dbresult = mysql_store_result(node.db_);
    if (!dbresult)
      break;
    /*
       MYSQL_FIELD * dbfield = 0;
       strresult += "#";
       while( (dbfield = mysql_fetch_field(dbresult)) )
       {
       strresult += string(dbfield->name) + "\t";
       }
       strresult += "\n";
     */
    MYSQL_ROW row;
    int numFields = mysql_num_fields(dbresult);
    int numRows = mysql_num_rows(dbresult);
    for (int i = 0; i < numRows; ++i) {
      if (i > 0)
        strresult += "\n";
      row = mysql_fetch_row(dbresult);
      for (int j = 0; j < numFields; j++) {
        if (j>0)
          strresult += "\t";
        strresult += (NULL ==row[j]) ? string("") : string(row[j]);
      }
    }
    mysql_free_result(dbresult);
    bret = true;
  } while (false);

  ReleaseConnection(node);
  return ( bret ? 0 : -1 );
}

int32_t DBPool::ExecuteTest(const string &strsql, string *pstrresult) {
  assert(pstrresult != NULL);
  string &strresult = *pstrresult;
  strresult.resize(0);
  DBConnectNode node;
  bool  bret = AcquireConnection(&node);
  if (!bret) {
    strresult += "AcquireConnection return false\n";
    return -2;
  }
  bret = false;
  int iret = 0;
  do {
    if (!node.is_connected_) {
      strresult += "node.is_connected_==false\n";
      break;
    }
    mysql_autocommit(node.db_, 1);
    strresult += "execute sql is:" + strsql + "\n";
    iret = mysql_query(node.db_, strsql.c_str());
    if (iret != 0) {
      iret = mysql_errno(node.db_);
    }
    if ((CR_ERROR_FIRST <= iret) && (iret <= CR_ERROR_LAST)) {
      string strtmp1 = mysql_error(node.db_);
      string strtmp2 = Text::IntToStr(mysql_errno(node.db_));
      mysql_close(node.db_);
      delete node.db_;
      node.db_ = 0;
      node.is_connected_ = false;
      strresult += "node.is_connected_ set to false, the errno is:" + strtmp2 + ", the errmsg is:" + strtmp1 + "\n";
      break;
    }
    if (iret != 0) {
      string strtmp1 = mysql_error(node.db_);
      string strtmp2 = Text::IntToStr(mysql_errno(node.db_));
      strresult += "query error, iret is:" + Text::IntToStr(iret) + ", the errno is:"
                  + strtmp2 + ", the errmsg is:" + strtmp1 + "\n";
      break;
    }
    MYSQL_RES * dbresult = mysql_store_result(node.db_);
    if (!dbresult) {
      strresult += "dbresult is null\n";
      break;
    }
    /*
     * if need head line, remove the comment.
     MYSQL_FIELD * dbfield = 0;
     strresult += "#";
     while( (dbfield = mysql_fetch_field(dbresult)) )
     {
     strresult += string(dbfield->name) + "\t";
     }
     strresult += "\n";
     */
    MYSQL_ROW row;
    int numFields = mysql_num_fields(dbresult);
    int numRows = mysql_num_rows(dbresult);
    for (int i = 0; i < numRows; ++i) {
      if (i > 0) {
        strresult += "\n";
      }
      row = mysql_fetch_row(dbresult);
      for (int j = 0; j < numFields; ++j) {
        if (j > 0) {
          strresult += "\t";
        }
        strresult += (NULL ==row[j]) ? string("") : string(row[j]);
      }
    }
    mysql_free_result(dbresult);
    bret = true;
  } while (false);
  strresult += "\nexecute sql test end! return is" + string((bret?"true":"false")) + "\n";
  ReleaseConnection(node);
  return (bret ? 0 : -1);
}

uint32_t DBPool::InsertAndGetid(const string &strsql) {
  DBConnectNode node;
  bool  bret = AcquireConnection(&node);
  if (!bret) {
    return -2;
  }
  bret = false;
  int iret = 0;
  do {
    if (!node.is_connected_) {
      break;
    }
    mysql_autocommit(node.db_, 1);
    iret = mysql_query(node.db_, strsql.c_str());
    if (iret != 0) {
      iret = mysql_errno(node.db_);
    }
    if ((CR_ERROR_FIRST <= iret) && (iret <= CR_ERROR_LAST)) {
      mysql_close(node.db_);
      delete node.db_;
      node.db_ = 0;
      node.is_connected_ = false;
      break;
    }
    if (iret != 0) {
      break;
    }
    iret = mysql_insert_id(node.db_);
    bret = true;
  } while (false);

  ReleaseConnection(node);
  return (bret ? iret : 0);
}

bool DBPool::AcquireConnection(DBConnectNode *pnode) {
  assert(pnode != NULL);
  DBConnectNode &node = *pnode;
  db_connection_stack_mutex_.Lock();
  if (db_connection_stack_.empty()) {
    db_connection_stack_mutex_.Unlock();
    return false;
  }
  DBConnectNode &snode = db_connection_stack_.top();
  node.db_ = snode.db_;
  node.is_connected_ = snode.is_connected_;
  node.connect_times_ = snode.connect_times_;
  db_connection_stack_.pop();
  db_connection_stack_mutex_.Unlock();
  if (node.is_connected_) {
    return true;
  }
  ++node.connect_times_;
  node.db_ = 0;
  node.db_ = new MYSQL;
  if (!node.db_) {
    return true;
  }
  mysql_init(node.db_);
  if (!mysql_real_connect(node.db_, db_host_.c_str(), db_usr_.c_str(),
                        db_pwd_.c_str(), db_name_.c_str(), db_port_int_, NULL, 0)) {
    mysql_close(node.db_);
    delete node.db_;
    node.db_ = 0;
    return true;
  }
  mysql_query(node.db_, "set names 'utf8'");
  node.is_connected_ = true;
  return true;
}

bool DBPool::ReleaseConnection(const DBConnectNode &node ) {
  MutexGuard guard(&db_connection_stack_mutex_);
  if (db_connection_stack_.size() >= db_pool_size_) {
    return false;
  }
  db_connection_stack_.push(node);
  return true;
}
} } }   // end of namespace ganji::util::db
