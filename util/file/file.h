/**
 * @Copyright 2009 Ganji Inc.
 * @file    ganji/util/file/file.h
 * @namespace ganji::util::file
 * @version 1.0
 * @author  haohuang
 * @date    2010-07-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_UTIL_FILE_FILE_H_
#define _GANJI_UTIL_FILE_FILE_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include "global.h"

using namespace std;

namespace ganji { namespace util { namespace file {

/// all functions here are static, so we use a namespace to warp these functions and make it looks like a class

  /// load file to string
  /// @param[in] path File name to be read
  /// @param[in] start_offset Start point to read
  /// @param[out] pcontent The string loaded
  /// @return Content load from file
  bool LoadFile(const std::string &path, size_t start_offset, std::string *pcontent);

  /// load list file, each line as a std::string, put instd::vector
  /// @note The algorithm is that: load whole file to memory, and segment the content to vector
  /// @param[in] path The file to be load
  /// @param[in] start_offset The start offset of file to load
  /// @param[out] presult The lines loaded
  bool LoadListFromFile(const std::string &path, size_t start_offset, std::vector< std::string > *presult);

  /// load lisr file, each line as a std::string, put instd::set
  /// @note The algorithm is that: load whole file to memory, and segment the content to set
  /// @param[in] path The file to be load
  /// @param[in] start_offset The start offset of file to load
  /// @param[out] presult The lines loaded
  bool LoadListFromFile(const std::string &path, size_t start_offset, std::set<std::string> *presult);

  /// check whether file is file or not
  /// @param[in] file file to be checked
  /// @return true:exist false:not
  bool IsFileExist(const std::string &file);

  
class File {
private:
  string file_name_;
  string file_mode_;
  string file_option_;
  FILE   *file_ptr_;
  size_t offset_;
public:
  File(const string &file_name, const string file_mode);
  ~File();
  bool StoreStringIntoFile(const string &content, size_t start_offset = 0);
  bool StoreMapOfArrayIntoFile(const std::map<std::string, std::vector<std::string> >*presult, size_t start_offset = 0);
};

 } } }   // end of namespace ganji::util::text::file
#endif  ///< _GANJI_UTIL_FILE_FILE_H_
