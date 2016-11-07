#include "commonconfig.h"
#include <stdio.h>
#include "config.h"
namespace ganji { namespace util { namespace config {
using std::string;

CommonConfig::CommonConfig(const char *path) {
  LoadConfig(path);
}

bool CommonConfig::LoadConfig(const char *path) {
  Config conf;
  conf.loadConfFile(path);

  Config::ItemMapVector_t &config_list = conf.getNamedList("config_list");
  for(Config::ItemMapVector_t::const_iterator it = config_list.begin();
      it !=config_list.end(); ++it) {
    string type;
    string name;
    Config::getItemValueInMap(*it, "type_", type, "");
    Config::getItemValueInMap(*it, "name_", name, "");
    if(type == "str") {
      string value;
      Config::getItemValueInMap(*it, "value_", value, "");
      str_configs_[name] = value;
    }
    else if(type == "int") {
      int value;
      Config::getItemValueInMap(*it, "value_", value, -1);
      int_configs_[name] = value;
    }
  }
  return true;
}
void CommonConfig::PrintConfig() const {
  for (std::map<string, string>::const_iterator cit = str_configs_.begin(); cit != str_configs_.end(); ++cit) {
    fprintf(stderr, "str config, %s:%s\n", cit->first.c_str(), cit->second.c_str());
  }
  for (std::map<string, int>::const_iterator it = int_configs_.begin(); it != int_configs_.end(); ++it) {
    fprintf(stderr, "int config, %s:%d\n", it->first.c_str(), it->second);
  }
}
} } }
