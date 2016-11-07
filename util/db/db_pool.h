/** 
 * @Copyright 2010 GanJi Inc.
 * @file    src/ganji/util/db/db_pool.h
 * @namespace ganji::util::db
 * @version 1.0
 * @author  huanghao
 * @date    2010-07-27
 * 
 * an implement of mysql database pool 
 *
 * Change Log:
 * 2009-10-25   add simple Query and Execute method
 *
 */

#ifndef _GANJI_UTIL_DB_DB_POOL_H_
#define _GANJI_UTIL_DB_DB_POOL_H_

#include <mysql/mysql.h>
#include <stack>
#include <vector>
#include <string>

#include "util/thread/mutex.h"
#include "global.h"

namespace ganji { namespace util { namespace db {
class SqlResult {
 public:
  SqlResult() {
    Init();
  }
  void Init() {
    error_no = 0;
    error_message.clear();
    insert_id = 0;
    affected_rows = 0;
    vec_field_name.clear();
    vec_record.clear();
  }
 public:
  int error_no;
  std::string error_message;
  uint64_t insert_id;
  uint64_t affected_rows;     ///< affected rows
  std::vector<std::string> vec_field_name;
  std::vector<std::vector<std::string> > vec_record;  ///< query result
};

/// @class DBPool
/// @brief data base pool
class DBPool {
 public:
  /// @brief Constructor
  /// @param[in] pool_size how many database pool to init, if no sure, set 10
  explicit DBPool(uint32_t pool_size);
  /// @brief Deconstructor
  ~DBPool();

  /// @brief escape string
  /// @param[in] input The string to be escaped
  /// @param[out] output The escaped string
  static bool EscapeString(const std::string &input, std::string *output);

  /// @brief initialize
  /// @param[in] h host
  /// @param[in] port port
  /// @param[in] u user
  /// @param[in] pwd pwd
  /// @param[in] n name
  /// @return If success, return true
  bool Initialize(const std::string &h, const std::string &port, const std::string &u,
                  const std::string &pwd, const std::string &n);

  /// @brief update
  /// @param[in] strsql sql string
  /// @param[out] SqlResult
  /// @return If success, return 0; else -1
  int32_t Execute(const std::string &strsql, SqlResult *result);
  
  /// @brief update
  /// @param[in] strsql sql string
  /// @param[out] result_pstr
  /// @return If success, return 0; else -1
  int32_t Execute(const std::string &strsql);

  /// @brief batch update
  /// @param[in] slq vector contains sql strings
  /// @return If success, return 0; else -1
  int32_t ExecuteBatch(const std::vector<std::string> &vecsql);

  /// @brief query
  /// @param[in] strsql Query sql string
  /// @param[out] SqlResult
  /// @return If success, return 0; else -1
  int32_t Query(const std::string &strsql, SqlResult *result);

  /// @brief query
  /// @param[in] strsql Query sql string
  /// @param[out] result_pstr
  /// @return If success, return 0; else -1
  int32_t Query(const std::string &strsql, std::string *result_pstr);

  /// @brief test db
  /// @param[in] strsql sql string
  /// @param[out] strresult result string with details
  /// @return If success, return 0; else -1
  int32_t ExecuteTest(const std::string &strsql, std::string *strresult);

  /// @brief insert and get AUTO_INCREMENT id
  /// @param[in] strsql sql string
  /// @return If success, return id; else return 0
  uint32_t InsertAndGetid(const std::string &strsql);

 private:
  /// @class DBConnectNode
  /// @brief connect to db
  class DBConnectNode {
    public:
      /// @brief Constructor
      DBConnectNode()
      :db_(NULL), is_connected_(false), connect_times_(0) {
      }
    public:
      MYSQL* db_;
      bool is_connected_;
      uint32_t connect_times_;
  };

 private:
  /// @brief get DBConnectNode from connection stack
  /// @param[out] pnode DBConnectNode connected to db
  /// @return If success to connect, return true
  bool AcquireConnection(DBConnectNode *pnode);

  /// @brief release DBConnectNode to connection stack
  /// @param[in] node DBConnectNode should be back to pool
  /// @return If success to release, return true
  bool ReleaseConnection(const DBConnectNode &node);

 private:
  std::string db_host_;
  std::string db_port_;
  uint32_t db_port_int_;
  std::string db_usr_;
  std::string db_pwd_;
  std::string db_name_;

  uint32_t db_pool_size_;

  // TODO: should use pointer
  std::stack<DBConnectNode> db_connection_stack_;
  // mutex for stack synchronize operations
  ganji::util::thread::Mutex db_connection_stack_mutex_;
};
} } }   // end of namespace ganji::util::db
#endif  // _GANJI_UTIL_DB_DB_POOL_H_
