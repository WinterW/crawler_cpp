#include "./config.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <stdarg.h>
#include "config_parser.h"

#define GETVALUE_STRING(filed_name, dest_var) \
  do{\
    conf_.getItemValue(filed_name, dest_var); \
    if(dest_var.empty()){ \
      error_str_ += filed_name + " not set\n"; \
      has_error_ = true;\
    }\
  } while (0)

#define GETVALUE_INT(filed_name, dest_var) \
  do { \
    conf_.getItemValue(filed_name, dest_var, -1);\
    if (dest_var == -1) { \
      error_str_ += filed_name + " not set\n";\
      has_error_ = true;\
    }\
  } while (0)


extern "C" {
  int conf_parse();
}
extern int conf_debug;
extern FILE *conf_in;
namespace ganji { namespace util { namespace config {
Config::Config(const std::string &file_path) {
  this->loadConfFile(file_path);
}

Config::Config() {
}

void Config::getItemValueInMap(const ItemMap_t &items, std::string item_name,
    std::string& item_value, std::string default_value) {
  ItemMap_t::const_iterator it;
  if((it = items.find(item_name)) != items.end()) {
    item_value = it->second.c_str();
  } else {
    item_value = default_value;
  }
}

void Config::getItemValueInMap(const ItemMap_t &items,
    std::string item_name, int &item_value, int default_value) {
  ItemMap_t::const_iterator it;
  if((it = items.find(item_name)) !=items.end()) {
    item_value = ::atoi(it->second.c_str());
  } else {
    item_value = default_value;
  }
}

void Config::getItemValue(std::string item_name,std::string& item_value,std::string default_value) {
  Config::getItemValueInMap(items_, item_name, item_value, default_value);
}

void Config::getItemValue(std::string item_name, int& item_value, int default_value) {
  Config::getItemValueInMap(items_, item_name, item_value, default_value);
}

void Config::trim(std::string& str) {
  std::string::size_type pos1 = str.find_first_not_of(" \t");
  std::string::size_type pos2 = str.find_last_not_of(" \t");
  str = str.substr(pos1 == std::string::npos ? str.length() - 1  : pos1, 
      pos2 == std::string::npos ? 0 : pos2 - pos1 + 1);	
}

void Config::loadConfFile(const std::string &file_path) {
  FILE *fp = fopen(file_path.c_str(), "r");
  if(!fp) {
    char msg[1024];

    snprintf(msg, sizeof(msg), "open config file failed, file_path == %s", file_path.c_str());
    throw ConfigException(msg);
    return;
  }

  conf_in = fp;
  cur_config = this;
  //conf_debug = 1;
  conf_parse();
  fclose(fp);
}

void Config::config_new_string_item(const char *key, const char *value)
{
  if(!key || !value)
  {
    return;
  }

  items_[key]=value;
}

void Config::config_new_list_record_item(const char * cur_key, const char *item_key, const char *item_value)
{
  if(!cur_key || !item_key || !item_value)
  {
    return;
  }
  parser_cur_record_[item_key] = item_value;
}

void Config::config_new_list_record(const char *key) {
  if(!key) {
    return;
  }
  // One record got, put it to named_list_
  named_list_[key].push_back(parser_cur_record_);
  parser_cur_record_.clear();
}

void Config::config_new_list(const char *key) {
  if(!key) {
    return;
  }
  // Nothing to do.
}

void Config::config_error(const char *msg) {
  throw ConfigException(msg);
}

/**
 *Just example, derived class should rewrite.
 */
bool ConfigHandle::LoadConfig(const char * path) {
  conf_.loadConfFile(path);

  if(has_error_) {
    fprintf(stderr, "Config error: \n%s\n", error_str_.c_str());
  }
  return !has_error_;
}

void ConfigHandle::GetIntValue(std::string  item_name,int & item) {
  GETVALUE_INT(item_name,item);
}

void ConfigHandle::GetStringValue(std::string  item_name,std::string & item) {
  GETVALUE_STRING(item_name,item);
}

void ConfigHandle::GetObjIntValue(std::string  obj_name, std::string  item_name,int & item) {
  GETVALUE_INT(obj_name + "_" + item_name,item);
}

void ConfigHandle::GetObjStringValue(std::string  obj_name, std::string  item_name,std::string & item) {
  GETVALUE_STRING(obj_name + "_" + item_name,item);
}
} } }
