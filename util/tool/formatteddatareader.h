/**
 * @Copyright 2010 Ganji Inc.
 * @file    src/ganji/util/tool/formatteddatareader.h
 * @namespace ganji::util::tool
 * @version 1.0
 * @author  jiafazhang
 * @date    2010-08-26
 *
 * Define a class to read \t \n formated data
 *
 * Change Log:
 *
 */
#ifndef _GANJI_UTIL_TOOL_FORMATEDDATAREADER_H_
#define _GANJI_UTIL_TOOL_FORMATEDDATAREADER_H_

namespace ganji { namespace util { namespace tool {
/*
 * @class FormatedDataReader
 *
 * @desc Read the data in file which have the same pattern
 */

class FormatedDataReader {
 public:
  /// @brief Default constructor
  /// @param[in] file_name The file path to read
  /// @param[in] fields The number of fields in one line
  FormatedDataReader(int fields);

  /// @brief Destructor
  ~FormatedDataReader();

  void OpenNewFile(const char *file_name);
  /// @brief Get the next line of data in the file
  /// @return Get the data successfully or not
  bool GetNextItem();

  /// @brief If the last line is not the data you want you can push the data back, the next read will return the data
  void PushBackData();

  const char *GetData(int index) const {
    return mem_buf + field_start_pos[index];
  }
 private:
  int *field_start_pos;     ///< The field start postion of each field in the buf
  char *mem_buf;            ///< The buf that contains the data
  static const int kBufSize = 256 * 1024;
  bool data_pushed_back_;   ///< Is the last data is pushed back
  bool end_;                ///< Whether the end of the file is read
  int field_num_;           ///< How many fields
  int fd_;                  ///< The file descriptor of the data file
  int index_;               ///< An inner used index number to record the postion to read
  int buf_len_;
};

} } }
#endif      ///< _GANJI_UTIL_TOOL_FORMATEDDATAREADER_H_
