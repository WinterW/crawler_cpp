#ifndef _GANJI_UTIL_CONFIG_EXCEPTIONS_H_
#define _GANJI_UTIL_CONFIG_EXCEPTIONS_H_

#include <stdexcept>
namespace ganji { namespace util { namespace config {
  class BaseException:public std::runtime_error
  {
    public:
    BaseException(const char *what)
      :runtime_error(what)
    {}
  };
} } }
#endif
