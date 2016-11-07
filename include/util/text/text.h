/**
 * @Copyright 2009 Ganji Inc.
 * @file    ganji/util/text/text.h
 * @namespace ganji::util::text
 * @version 1.0
 * @author  haohuang
 * @date    2010-07-21
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_UTIL_TEXT_TEXT_H_
#define _GANJI_UTIL_TEXT_TEXT_H_

#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <utility>
#include "global.h"

namespace ganji { namespace util { namespace text {

/// all functions here are static, so we use a namespace to warp these functions and make it looks like a class
namespace Text {
  /// 把string类型转换为其他类型（该类型应支持<<操作）
  /// @param[in] s 待转换的string
  /// @param[in] radix 指定转换结果使用的基数，可以取值16，8，或其它。
  /// 如果选择其他，会使用10进制作为结果基数。
  /// @return 返回从string s转换得到的结果，该结果的类型是T。
  template <typename T> T FromString(const std::string &s, uint16_t radix);

  /// @brief 把T类型(该类型应支持>>操作）转换为string类型
  /// @param[in] t 待转换的T实例
  /// @param[in] radix 指定转换结果使用的基数，可以取值16，8，或其它。
  /// 如果选择其他，会使用10进制作为结果基数。
  /// @return 返回从T转换得到的结果，该结果的类型是string。
  template <typename T> std::string ToString(const T &t, uint16_t radix);
  
  /// 字符串转化为int32_t
  /// @param[in] value 待转化的string
  /// @return 返回转化后的int32_t
  int32_t StrToInt(const std::string &str_value);

  /// 字符串转化为int64_t
  /// @param[in] value 待转化的string
  /// @return 返回转化后的int64_t
  int64_t StrToInt64(const std::string &str_value);
  
  /// @brief 字符串转化为double
  /// @param[in] value 待转化的string
  /// @return 返回转化后的double
  double StrToDouble(const std::string &str_value);
  
  /// 把int32_t转化成string
  /// @param[in] value 带转化的数
  /// @return 转化后的结果。例如IntToStr(123) 会得到字符串"123"
  std::string IntToStr(int32_t value);

  /// 把uint32_t转化成string
  /// @param[in] value 带转化的数
  /// @return 转化后的结果。例如UIntToStr(123) 会得到字符串"123"
  std::string UIntToStr(int32_t value);

  
  /// 把int64_t转化成string
  /// @param[in] value 带转化的数
  /// @return 转化后的结果。例如IntToStr(123) 会得到字符串"123"
  std::string Int64ToStr(int64_t value);

  /// 把uint64_t转化成string
  /// @param[in] value 带转化的数
  /// @return 转化后的结果。例如UIntToStr(18446744073031723246) 会得到字符串"18446744073031723246"
  std::string UInt64ToStr(uint64_t value);

  /// double 类型转化为std::string
  /// @param[in] value 带转化的double
  /// @return 返回转化后的string
  std::string DoubleToStr(double value);
  
  /// @brief 对输入的input句子， 用seperator进行分割. 将分割的结果按顺序写入segments
  ///       如果有两个连续的用seperator， 则会插入入segments一个空字符串
  /// @param[in] input,  用于分割的句子
  /// @param[in] seperator 分割字符
  /// @param[out] segments, 存储分割结果的vector
  /// @return void
  void Segment(const std::string &input, char seperator, std::vector<std::string> *segments);

  /// 与Segment相同， 区别在于分隔词从字符变为了了字符
  void Segment(const std::string &input, const std::string &seperator, std::vector<std::string> *segments);
  
  /// trim功能， 当前还不支持对中文空格的trim，以后会考虑加入
  /// @param[in,out] str is source string
  /// @return void
  void Trim(std::string *pstr);

  /// strim 特殊的字符串 前后都trim
  /// @param[in] remstr is the char set should be removed at head and tail parts
  /// @param[in,out] str is source string
  /// @return void
  void Trim(const std::string &remstr, std::string *pstr);

  /// @brief Remove all leading occurrences of a set of specified characters from the source string(all_str) in-place.
  /// @param[in] all_str is the char set should be removed at head and tail parts
  /// @param[in,out] pstr is source string
  /// @return void
  void LTrimAll(const std::string &all_str, std::string *pstr);

  /// @brief Remove all tail occurrences of a set of specified characters from the source string(all_str) in-place.
  /// @param[in] all_str is the char set should be removed at head and tail parts
  /// @param[in,out] pstr is source string
  /// @return void
  void RTrimAll(const std::string &all_str, std::string *pstr);

  /// @brief Remove all leading and tail occurrences of a set of specified characters from the source string(all_str) in-place.
  /// @param[in] all_str is the char set should be removed at head and tail parts
  /// @param[in,out] *pstr is source string
  /// @return void
  void TrimAll(const std::string &allstr, std::string *pstr);
  
  /// 把str中的str_pattern替换成str_replace 
  /// @param[in] str_pattern is the sub string to be str_pattern
  /// @param[in] str_replace is the string which is ued to replace the orign string
  /// @param[in,out] pstr is source string
  /// @return Return the number of str_pattern string. If str or str_pattern is empty, return -1
  int  ReplaceStrStrSameLen(const std::string &str_pattern, const std::string &str_replace, std::string *pstr);

  /// 把str中的str_pattern替换成str_replace 
  /// @param[in] str_pattern is the sub string to be str_pattern
  /// @param[in] str_replace is the string which is ued to replace the orign string
  /// @param[in,out] pstr is source string
  /// @return Return the number of str_pattern string. If str or str_pattern is empty, return -1
  int  ReplaceStrStr(const  std::string &str_pattern, const std::string &str_replace, std::string *pstr);

  /// 将一系列字符串换成指定字符串
  /// @param[in] vec_replaced is the sub string set to be str_pattern
  /// @param[in] str_replace is the string which is ued to replace the orign string
  /// @param[in] ntype specify whether use safe method. If ntype is nonzero, use safe method
  /// @param[in,out] str is source string
  /// @return Return the number of str_pattern string. If str or str_pattern is empty, return -1
  int  ReplaceStrVec(const  std::vector<std::string>  &vec_pattern, const std::string &str_replace, std::string *pstr);

  /// 获取字符串str_content中str_word出现的次数
  int GetWordCount(const std::string &str_content, const std::string &str_word);

  template <typename T> T FromString(const std::string &s, uint16_t radix) {
    std::istringstream iss(s);
    T t = 0; //default value is 0, by david 
    if (16 == radix)
      iss >> std::hex >> t;
    else if (8 == radix)
      iss >> std::oct >> t;
    else
      iss >> t;
    return t;
  };

  template <typename T> std::string ToString(const T &t, uint16_t radix) {
    std::ostringstream oss;
    if (16 == radix)
      oss << std::hex << t;
    else if (8 == radix)
      oss << std::oct << t;
    else
      oss << t;
    return oss.str();
  };

  /// @brief 字符串循环左移
  /// @param[in/out] p_str 待处理的字符串
  /// @param[in] rotate_step 循环左移的step
  void LeftRotate(std::string *p_str, int rotate_step);

  /// @brief 字符串首尾交换
  /// @param[in/out] p_str 待处理的字符串
  void ReverseStr(std::string *p_str);

  /// @brief 字符串中所有字母修改为小写
  /// @param[in/out] p_str 待处理的字符串
  void ToLower(std::string *p_str);

  /// @brief 字符串中所有字母修改为大写
  /// @param[in/out] p_str 待处理的字符串
  void ToUpper(std::string *p_str);

}
} } }   // end of namespace ganji::util::text::Text
#endif  // _GANJI_UTIL_TEXT_TEXT_H_
