/**
 * @Copyright 2011 Ganji Inc.
 * @file    src/util/tool/stable_sparse_hash_map.h
 * @version 1.0
 * @author  zhangjiafa
 * @date    2011-01-23
 *
 * @brief
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_TOOL_STABLE_SPARSE_HASH_MAP_H_
#define _GANJI_UTIL_TOOL_STABLE_SPARSE_HASH_MAP_H_

#include <sys/stat.h>
#include <string>
#include <queue>
#include <google/sparse_hash_map>

#include "ganji/util/thread/mutex.h"
using google::sparse_hash_map;
using namespace HASH_NAMESPACE;

namespace ganji { namespace util { namespace tool {

template <typename T>
class StableSparseHashMap {
 public:
  // 对load_from_old_file字段的说明，-1 不载入 0,1 指定从其中的特定的文件载入 2 载入数据，从老的数据文件
  int Init(const std::string &file_path, int load_from_old_file) {
    file_names_[0] = file_path + std::string(".1");
    file_names_[1] = file_path + std::string(".2");
    last_save_file_ = 0;
    if (load_from_old_file >= 0) {
      //判断哪个文件是可靠的文件，选择那个时间更老的
      last_save_file_ = load_from_old_file;
      if (last_save_file_ > 1) {
        struct stat f_stat1, f_stat2;
        int r1 = stat(file_names_[0].c_str(), &f_stat1);
        int r2 = stat(file_names_[1].c_str(), &f_stat2);
        fprintf(stderr, "%u %u\n", f_stat1.st_mtime, f_stat2.st_mtime);
        if (r1 == -1)
          last_save_file_ = 1;
        else if (r2 == -1)
          last_save_file_ = 0;
        else
          last_save_file_ = f_stat1.st_mtime > f_stat2.st_mtime ? 1 : 0;
      }
      FILE *fp = fopen(file_names_[last_save_file_].c_str(), "rb");
      if (fp != NULL) {
        dump_map_.read_metadata(fp);
        dump_map_.read_nopointer_data(fp);
        fclose(fp);
      }
    }
    return last_save_file_;
  }
  void UpdateData(typename T::value_type data) {
    queue_mutex_.Lock();
    dump_queue_.push(std::make_pair(1, data));
    queue_mutex_.Unlock();
  }
  void AddData(typename T::value_type data) {
    queue_mutex_.Lock();
    dump_queue_.push(std::make_pair(0, data));
    queue_mutex_.Unlock();
  }
  void DelData(typename T::value_type data) {
    queue_mutex_.Lock();
    dump_queue_.push(std::make_pair(2, data));
    queue_mutex_.Unlock();
  }
  void SaveData(int max_cnt) {
    int finish_cnt = 0;
    queue_mutex_.Lock();
    while (finish_cnt <= max_cnt && !dump_queue_.empty()) {
      std::pair<int, typename T::value_type > a = dump_queue_.front();
      dump_queue_.pop();
      switch (a.first) {
        case 0:
          dump_map_.insert(a.second);
          break;
        case 1:
          dump_map_[a.second.first] = a.second.second;
          break;
        default:
          {
            typename T::iterator it = dump_map_.find(a.second.first);
            if (it != dump_map_.end()) {
              dump_map_.erase(it);
              break;
            }
          }
          ++finish_cnt;
      }
    }
    queue_mutex_.Unlock();
    last_save_file_ = last_save_file_ == 1 ? 0 : 1;
    FILE *fp = fopen(file_names_[last_save_file_].c_str(), "wb+");
    if (fp) {
      dump_map_.write_metadata(fp);
      dump_map_.write_nopointer_data(fp);
      fclose(fp);
    }
  }
  T dump_map_;
 protected:
  ganji::util::thread::Mutex queue_mutex_;         ///< The mutex
  std::queue<std::pair<int, typename T::value_type > > dump_queue_;
  int last_save_file_;
  std::string file_names_[2];
};
} } }
#endif  ///< _GANJI_UTIL_TOOL_STABLE_SPARSE_HASH_MAP_H_
