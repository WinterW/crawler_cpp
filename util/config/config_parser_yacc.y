%{
#define TRUE 1
#define FALSE 0
#include "config_parser.h"
#include <string.h>

typedef char* cstring_t; /* to specify token types as char* */
#define YYSTYPE cstring_t /* some code*/

const char *cur_key;
%}

%debug
%name-prefix="conf_"
%error-verbose
%token STRING
%%

begin: confs{
};

confs: conf
  | confs conf{
  };
  
conf:simple_conf | list_conf  {
} ;

simple_conf: key '=' STRING{
      config_new_string_item($1, $3);
     };

list_conf: key '=' list_value {
             config_new_list($1);
           };
list_value: '[' records ']' {
           };

key: STRING {
             cur_key=$1;
           };
records: /* empty */
  | record 
  | records ',' record {
  };

record: '{' items '}' {
        config_new_list_record(cur_key);
      };

items: item
  | items ',' item {
  };

item: STRING '=' STRING{
        config_new_list_record_item(cur_key, $1, $3);
      };

%%

int yyerror(char *msg)
{
  config_error(msg);
  return 0;
}

