/**
* @Copyright 2011 Ganji Inc.
* @file    ganji/util/file/fmd5.h
* @namespace ganji::util::file
* @version 1.0
* @author  lihaifeng
* @date    2011-07-18
* @effect  得到文件内容的md5值，进而来判断一个文件内容是否发生改变
*
* 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
* 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
* Change Log:
*/

#ifndef _GANJI_UTIL_FILE_FMD5_H_
#define _GANJI_UTIL_FILE_FMD5_H_
#include <string>

using std::string;

namespace ganji { namespace util { namespace file {

namespace FMD5 {

///   md5加密
void md5();

/// 计算文件内容md5值,应用程序直接调用此函数可以得到一个文件内容的MD5值
/// @param[in] file_name：文件名
/// @param[out] file_content  文件内容的md5值
void FileContetMd5(const string &file_name, string &file_content);


}

} } }

#endif ///< GANJI_UTIL_FILE_FMD5_H_
