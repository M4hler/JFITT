%{
#include <iostream>
#include "grammar.hpp"
#include "Definitions.hpp"
int yylex();
//void yyterminate();
extern long long lineNumber;
using namespace std;
%}

%x COMMENT

cmtstart \[
cmtend \]

cmteat [^]]*

%option noyywrap

%%

{cmtstart} BEGIN COMMENT;
<COMMENT>{cmteat}
<COMMENT>{cmtend} BEGIN INITIAL;

"DECLARE" {return DECLARE;}
"BEGIN" {return KW_BEGIN;} //BEGIN macro is already defined
"END" {return END;}

"READ" {return READ;}
"WRITE" {return WRITE;}

"ASSIGN" {return ASSIGN;}

"PLUS" {return PLUS;}
"MINUS" {return MINUS;}
"TIMES" {return TIMES;}
"DIV" {return DIV;}
"MOD" {return MOD;}

"IF" {return IF;}
"THEN" {return THEN;}
"ENDIF" {return ENDIF;}

"EQ" {return EQ;}
"NEQ" {return NEQ;}
"LE" {return LE;}
"GE" {return GE;}

[_a-z]+ {yylval.str = (char *)strdup(yytext); return PIDENTIFIER;}
-?[0-9]+ {yylval.num = atoi(yytext); return NUMBER;}

\n {lineNumber++;}

\( {return '(';}
\) {return ')';}
: {return ':';}
; {return ';';}
, {return ',';}

[[:blank:]]*
%%
