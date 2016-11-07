/**
 * @Copyright 2010 Ganji Inc.
 * @file    src/ganji/util/tool/shareresourcewrapper.h
 * @namespace ganji::util::tool
 * @version 1.0
 * @author  jiafazhang
 * @date    2010-08-26
 *
 * Define a class wrap resource that shared by many threads
 *
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_TOOL_SHARERESOURCEWRAPPER_H_
#define _GANJI_UTIL_TOOL_SHARERESOURCEWRAPPER_H_

#include "util/thread/mutex.h"
#include <vector>

namespace ganji { namespace util { namespace tool {

/*
 * @class ShareResourceWrapper
 *
 * @desc This is an utility class to wrap some share resource
 */
class ShareResourceWrapper {
 public:
  /// @brief Default constructor
  ShareResourceWrapper() : is_init_(false), res_num_(-1) {
  }

  /// @brief Destructor
  ~ShareResourceWrapper() {
  }

  /// @brief Init the class
  /// @param[in] number The number of resources it manage
  /// @return Init successfully or not
  bool Init(int number);

  /// @brief Get one of the resource
  /// @param[in] wait_time The max wait time, 0 wait util get the resource
  /// @return The id of the resource, -1 for no resource get
  int GetResource(int wait_time);

  /// @brief Release the resource
  /// @param[in] number The id of the resource
  /// @return Return the resouce successfully or not
  bool ReleaseResource(int resoureID);

  /// @brief Get one of the available resource
  /// @return The id of the resource, -1 for no resource get
  int GetAvailableResourceNum() {
    return available_num;
  }

 private:
  /// @brief Get one of the available resource
  /// @return The id of the resource, -1 for no resource get
  int GetAvailableResource() const {
    for (int i = 0; i < res_num_; ++i) {
      if (res_state_[i]) {
        return i;
      }
    }
    return -1;
  }

 private:
  bool is_init_;                      ///< Is the object init
  int res_num_;                       ///< The resource number
  int available_num;                   ///< The available number
  ganji::util::thread::Mutex res_mutex_;         ///< The mutex
  std::vector<bool> res_state_;       ///< The resource state
};
} } }
#endif  ///< _GANJI_UTIL_TOOL_SHARERESOURCEWRAPPER_H_
