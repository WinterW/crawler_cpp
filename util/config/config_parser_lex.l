%{
  typedef char* cstring_t;
#define YYSTYPE cstring_t
#include "config_parser_yacc.h"
%}

%option prefix="conf_" outfile="lex.yy.c"
%%
^#.*$ {/* comment do nothing */}
[ \n\r\t] {/* do nothing */}
[^\[\]\{\}=, \n\r\t]+ conf_lval=strdup(yytext); return STRING;
. return yytext[0];
%%

int conf_wrap()
{
  return 1;
}
