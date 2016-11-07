#ifndef _GANJI_UTIL_CONFIG_CNFIG_H_
#define _GANJI_UTIL_CONFIG_CNFIG_H_

#include <string>
#include <map>
#include <vector>

#include "global.h"

/**
* @brief load configure file from disk 
*
*/

#include  <stdint.h>
#include "exceptions.h"
namespace ganji { namespace util { namespace config {
#define SYMBOL_2_STRING(x) (#x)

class ConfigException: public BaseException {
 public:
  ConfigException(const char * what):BaseException(what){}
};

class Config {
 public:
  typedef std::map<std::string, std::string> ItemMap_t;
  typedef std::vector<ItemMap_t> ItemMapVector_t;
  typedef std::map<std::string, ItemMapVector_t> NamedList_t;

  Config(const std::string &file_path);
  Config();

  void loadConfFile(const std::string &file_path);	
  void getItemValue(std::string item_name, std::string& item_value, std::string default_value = "");
  void getItemValue(std::string item_name, int& item_value, int default_value = 0);
  void getItemValue(std::string item_name, uint32_t& item_value, uint32_t default_value = 0) {
    int tmp_val = 0;
    getItemValue(item_name, tmp_val, (int)default_value);
    item_value = tmp_val;
  }

  std::vector<std::map<std::string, std::string> > & getNamedList(std::string item_name) {
    return named_list_[item_name];
  }


  static void getItemValueInMap(const ItemMap_t &items,
      std::string item_name, std::string& item_value, std::string default_value);
  static void getItemValueInMap(const ItemMap_t &items,
      std::string item_name, int &item_value, int default_value);

  /*-----------------------------------------------------------------------------
   *  used for c parser to call, DO NOT use these methods unless
   *  you know what you are doing.
   *-----------------------------------------------------------------------------*/
  void config_new_string_item(const char *key, const char *value);
  void config_new_list(const char *key);
  void config_new_list_record_item(const char * cur_key, const char *item_key, const char *item_value);
  void config_new_list_record(const char *key);
  void config_error(const char *msg);
 private:
  ItemMap_t parser_cur_record_; /* used to temp store record currently parsing*/

  /**
   * Store config like: servers=[{ip=127.0.0.1, port=12345}, {ip=127.0.0.1, port=54321}]
   */
  NamedList_t named_list_;

  /**
   * map<item_name,item_value>
   */ 
  std::map<std::string,std::string> items_;

  void trim(std::string& val);
};

class ConfigHandle {
 public:
  virtual ~ConfigHandle() {
  };
  virtual bool LoadConfig(const char * path);

  void GetIntValue(std::string  item_name,int & item);
  void GetStringValue(std::string  item_name,std::string & item);

  void GetObjIntValue(std::string  obj_name, std::string item_name,int & item);
  void GetObjStringValue(std::string  obj_name,std::string item_name,std::string & item);

 protected:
  ConfigHandle() {
  };
  Config conf_;
  std::string error_str_;
  bool has_error_;
  std::string int_item_list; // e.g. "int_var1 int_var2"
  std::string string_item_list;//e.g. "string_var1 string_var2"
};
} } }
#endif  ///<_GANJI_UTIL_CONFIG_CNFIG_H_
