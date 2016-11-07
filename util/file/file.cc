/**
 * @Copyright 2009 Ganji Inc.
 * @file    ganji/util/file/file.cc
 * @namespace ganji::util::file
 * @version 1.0
 * @author  haohuang
 * @date    2010-07-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <assert.h>
#include "util/log/thread_fast_log.h"

namespace FastLog = ::ganji::util::log::ThreadFastLog;
using FastLog::WriteLog;
using FastLog::kLogFatal;
using FastLog::kLogDebug;
using FastLog::kLogNotice;
using FastLog::kLogWarning;

namespace ganji { namespace util { namespace file { 
bool LoadFile(const string &path, size_t start_offset, string *pcontent) {
  assert(pcontent != NULL);
  string &content = *pcontent;
  content = "";
  FILE *f = fopen(path.c_str(), "rb");
  if (f == NULL) {
    return false;
  } else {
    fseek(f, start_offset, SEEK_SET);
    char buf[65536];
    int readLen = 0;
    while ((readLen = fread(buf, 1, 65536, f)) > 0) {
      content.append(buf, readLen);
    }
    fclose(f);
    return true;
  }
}

bool LoadListFromFile(const string &path, size_t start_offset, vector<string> *presult) {
  assert(presult != NULL);
  presult->clear();
  string content;
  if (LoadFile(path, start_offset, &content) == false)
    return false;
  for (size_t start_pos = 0, stop_pos = 0; stop_pos < content.size(); ++stop_pos) {
    const char &ch = content[stop_pos];
    if (ch == '\r'|| ch == '\n' || stop_pos == content.size() - 1) {
      if (start_pos != stop_pos)
        presult->push_back(content.substr(start_pos, stop_pos-start_pos));
      start_pos = stop_pos + 1;
    }
  }
  return true;
}

bool LoadListFromFile(const string &path, size_t start_offset, set<string> *presult) {
  assert(presult != NULL);
  presult->clear();
  string content;
  if (LoadFile(path, start_offset, &content) == false)
    return false;
  for (size_t start_pos = 0, stop_pos = 0; stop_pos < content.size();++stop_pos) {
    const char &ch = content[stop_pos];
    if (ch == '\r'|| ch == '\n'|| stop_pos == content.size()-1) {
      if (start_pos != stop_pos)
        presult->insert(content.substr(start_pos, stop_pos-start_pos));
      start_pos = stop_pos+1;
    }
  }
  return true;
}

bool IsFileExist(const string &file) {
  struct stat info;
  if (0 == stat(file.c_str(), &info)) {
    return S_ISREG(info.st_mode);
  }
  return false;
}
File::File(const string &file_name, const string file_mode):file_name_(file_name), file_mode_(file_mode) {
     if (IsFileExist(file_name_))
	remove(file_name_.c_str());         
     if ((file_ptr_ = fopen(file_name_.c_str(), file_mode_.c_str())) == NULL)
       printf("open file[%s:%s] failed!", file_name_.c_str(), file_mode_.c_str()); 
}
File::~File(){
     if (file_ptr_ != NULL)
     {
     	fflush(file_ptr_);
     	fclose(file_ptr_);
     }
}
bool File::StoreStringIntoFile(const string &content, size_t start_offset)
{
  int str_len = content.size();
  if (str_len == 0)
  	return true;
  if (file_ptr_ == NULL) {
    if ((file_ptr_ = fopen(file_name_.c_str(), file_mode_.c_str())) == NULL)
        {
          printf("open file[%s:%s] failed!", file_name_.c_str(), file_mode_.c_str());
          return false;
        }
  } else {
    if (start_offset != 0)
        fseek(file_ptr_, start_offset, SEEK_SET);
    int write_length = 0;
    fwrite("[", 1, 1, file_ptr_);
    if((write_length = fwrite(const_cast<char*>(content.c_str()), 1, str_len, file_ptr_)) != str_len) {
         //printf("write file[%s] error:[string length:%d != write length:%d]", file_name_.c_str(), str_len, write_length);
         ;
    }
    else
    {
         //printf("write file[%s]",content.c_str());
         ;
    }
    fwrite("]\n", 1, 2, file_ptr_);
    fflush(file_ptr_);
    return true;
  }
   
}
bool File::StoreMapOfArrayIntoFile(const std::map<std::string, std::vector<std::string> >*presult, size_t start_offset)
{
  if (presult == NULL)
    {
	WriteLog(kLogDebug, "File::StoreMapOfArrayIntoFile result empty");
        return true;
    }
  if (file_ptr_ == NULL) {
    if ((file_ptr_ = fopen(file_name_.c_str(), file_mode_.c_str())) == NULL)
        {
          //printf("open file[%s:%s] failed!", file_name_.c_str(), file_mode_.c_str());
          WriteLog(kLogDebug, "open file[%s:%s] failed!", file_name_.c_str(), file_mode_.c_str());
    	  return false;
        }
  } else {
    if (start_offset != 0)
    	fseek(file_ptr_, start_offset, SEEK_SET);
    char buf[65536];
    int str_len = 0;

    string buffer;
    buffer.append("{");
    //str_len += sprintf(buf, "{");
    for ( map<std::string, std::vector<std::string> >::const_iterator map_it = presult->begin();
          map_it != presult->end(); ++map_it)
    {
        //str_len += sprintf(buf, " {%s:", map_it->first.c_str());
        buffer.append("{");
        buffer.append(map_it->first);
        buffer.append(":");
        for (std::vector<std::string>::const_iterator vec_it = map_it->second.begin();
             vec_it != map_it->second.end(); ++vec_it)
        {
    	     //str_len += sprintf(buf, " %s", vec_it->c_str());  		 
             buffer.append(*vec_it);
             buffer.append(" ");
        } 
        //str_len += sprintf(buf, "} ");
        buffer.append("}");
    }
    //str_len += sprintf(buf, " }%c", '\0');
    buffer.append("}");
    int write_length = 0;
    WriteLog(kLogDebug, "File::StoreMapOfArrayIntoFile[%s]", buffer.c_str());
    //if((write_length = fwrite(buf, 1, str_len, file_ptr_)) != str_len) {
    if((write_length = fputs(const_cast<char*>(buffer.c_str()), file_ptr_)) != str_len) {
         //printf("write file[%s] error:[string length:%d != write length:%d]", file_name_.c_str(), str_len, write_length);
         //WriteLog(kLogDebug, "write file[%s] error:[string length:%d != write length:%d]", file_name_.c_str(), str_len, write_length);
         ; 
    }
    /*
    else
    {
         WriteLog(kLogDebug, "write file[%s]",buf);
    }
    */
    fflush(file_ptr_);
    return true;
  }
}
 } } }   ///< end of namespace ganji::util::file
