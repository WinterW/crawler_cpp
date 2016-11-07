/**
 * @Copyright 2010 Ganji Inc.
 * @file    src/ganji/util/tool/formateddatareader.cc
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
#include "formatteddatareader.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

namespace ganji { namespace util { namespace tool {

FormatedDataReader::FormatedDataReader(int fields) {
  mem_buf = new char[kBufSize];
  field_start_pos = new int[fields];
  field_num_ = fields;
  fd_ = -1;
  buf_len_ = kBufSize;
}

void FormatedDataReader::OpenNewFile(const char *file_name) {
  if (fd_ > 1) {
    close(fd_);
    fd_ = -1;
  }
  fd_ = open(file_name, O_RDONLY);
  data_pushed_back_ = false;
  if (fd_ <= 1)
    end_ = true;
  else
    end_ = false;
  index_ = kBufSize;
}

bool FormatedDataReader::GetNextItem() {
  if (data_pushed_back_) {
    data_pushed_back_ = false;
    return true;
  }
  if (end_)
    return false;

  int field_index = 0;
  field_start_pos[0] = index_;
  while (true) {
    for (; index_ < buf_len_; ++index_) {
      if (mem_buf[index_] == '\n') {
        mem_buf[index_] = '\0';
        ++index_;
        // if (field_index != field_num_ - 1)
          // fprintf(stderr, "wrong data format %d\n", field_num_);
        return true;
      } else if (mem_buf[index_] == '\t') {
        mem_buf[index_] = '\0';
        field_start_pos[++field_index] = index_ + 1;
      } else if (mem_buf[index_] == '\0') {
        mem_buf[index_] = ' ';
      }
    }
    // copy the last part to the head of the buf
    int last_half_line = kBufSize - field_start_pos[0];
    if (last_half_line > 0) {
      memcpy(mem_buf, mem_buf + field_start_pos[0], last_half_line);
    }
    index_ = last_half_line;
    // reset the positions
    for (int i = 1; i <= field_index; ++i)
      field_start_pos[i] -= field_start_pos[0];
    field_start_pos[0] = 0;
    int read_size;
    if ((read_size = read(fd_, mem_buf + last_half_line, kBufSize - last_half_line)) > 0) {
      if (read_size < kBufSize - last_half_line) {
        buf_len_ = last_half_line + read_size;
      }
      continue;
    } else {
      end_ = true;
      return false;
    }
  }
  end_ = true;
  return false;
}

void FormatedDataReader::PushBackData() {
  data_pushed_back_ = true;
}

FormatedDataReader::~FormatedDataReader() {
  close(fd_);
  delete[] mem_buf;
  delete[] field_start_pos;
}
} } }
