#ifndef _GANJI_UTIL_CONFIG_COMMONCONFIG_H_
#define _GANJI_UTIL_CONFIG_COMMONCONFIG_H_

#include <string>
#include <map>
namespace ganji { namespace util { namespace config {
class CommonConfig {
 public:
  CommonConfig(const char *path);
  bool LoadConfig(const char *path);
  void PrintConfig() const;
  std::map<std::string, std::string> str_configs_;
  std::map<std::string, int> int_configs_;
};
} } }
#endif  ///< end of _GANJI_UTIL_CONFIG_COMMONCONFIG_H_
