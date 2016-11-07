#ifndef  CONFIG_PARSER_INC
#define  CONFIG_PARSER_INC

#ifdef __cplusplus
namespace ganji { namespace util { namespace config {
  class Config;
} } }
extern ganji::util::config::Config *cur_config;
extern "C"{
#endif
void config_new_string_item(const char *key, const char *value);
void config_new_list(const char *key);
void config_new_list_record_item(const char * cur_key, const char *item_key, const char *item_value);
void config_new_list_record(const char *key);
void config_error(const char *msg);

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef CONFIG_PARSER_INC  ----- */
