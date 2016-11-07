#include "config_parser.h"
#include <stdio.h>
#include "config.h"
ganji::util::config::Config *cur_config = NULL;
extern "C"{

void config_new_string_item(const char *key, const char *value)
{
  cur_config->config_new_string_item(key, value);
}
void config_new_list(const char *key)
{
  cur_config->config_new_list(key);
}
void config_new_list_record_item(const char * cur_key, const char *item_key, const char *item_value)
{
  cur_config->config_new_list_record_item(cur_key, item_key, item_value);
}
void config_new_list_record(const char *key)
{
  cur_config->config_new_list_record(key);
}

void config_error(const char *msg)
{
  cur_config->config_error(msg);
}

}
