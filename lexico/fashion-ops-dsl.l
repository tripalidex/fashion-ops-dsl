%{
#include "fashion-ops-bison.tab.h"
#include <stdio.h>
#include <stdlib.h>
%}

%%

[ \t\r]+        ;
\n              { yylineno++; }
"//".*          ;

"REPORT"        { return REPORT; }
"STOCK"         { return STOCK; }
"REORDER"       { return REORDER; }
"SLOW"          { return SLOW; }
"DISCOUNT"      { return DISCOUNT; }
"IF"            { return IF; }
"THEN"          { return THEN; }
"ELSE"          { return ELSE; }
"FOR"           { return FOR; }
"IN"            { return IN; }
"VAR"           { return VAR; }
"CONST"         { return CONST; }
"ARRAY"         { return ARRAY; }
"END"           { return END; }
"SALES"         { return SALES; } 
"DAILY"         { return DAILY; } 
"MONTHLY"       { return MONTHLY; }
"FILTER"        { return FILTER; }
"IS"            { return IS; }
"DAYS"          { return DAYS; }
"%"             { return PERCENT; }
"DO"            { return DO; }

"=="            { return EQ; }
"!="            { return NEQ; }
">="            { return GEQ; }
"<="            { return LEQ; }
">"             { return GT; }
"<"             { return LT; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULT; }
"/"             { return DIV; }

"AND"           { return AND; }
"OR"            { return OR; }
"NOT"           { return NOT; }

"="             { return ASSIGN; }

";"             { return SEMICOLON; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
","             { return COMMA; }

[0-9]+(\.[0-9]+)? {
    yylval.fval = atof(yytext);
    return NUMBER;
}

\"[^"]*\"        {
    yylval.sval = strdup(yytext);
    return STRING;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.sval = strdup(yytext);
    return ID;
}

.               {
    fprintf(stderr, "Error léxico en la línea %d: Carácter inesperado '%s'\n", yylineno, yytext);
    exit(1);
}

%%

int main() {
    yylex()
    return 0;
}

int yywrap() {
    return 1;
}
