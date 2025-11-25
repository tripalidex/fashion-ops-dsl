%{
#include "fashion-ops-bison.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%option yylineno
%option noyywrap

%%

[ \t\r]+                  ;  /* ignorar espacios y tabuladores */
\n                        { yylineno++; }
"//".*                    ;  /* comentarios de una línea */

"REPORTAR"                { return REPORTAR; }
"VENTAS"                  { return VENTAS; }
"DIARIO"                  { return DIARIO; }
"MENSUAL"                 { return MENSUAL; }
"FILTRAR"                 { return FILTRAR; }

"VER_STOCK"               { return VER_STOCK; }
"REPONER"                 { return REPONER; }
"IDENTIFICAR_LENTOS"      { return IDENTIFICAR_LENTOS; }
"APLICAR_DESCUENTO"       { return APLICAR_DESCUENTO; }

"SI"                      { return SI; }
"ENTONCES"                { return ENTONCES; }
"SINO"                    { return SINO; }
"FIN"                     { return FIN; }

"PARA"                    { return PARA; }
"EN"                      { return EN; }
"HAZ"                     { return HAZ; }

"VARIABLE"                { return VARIABLE; }
"CONSTANTE"               { return CONSTANTE; }
"ARREGLO"                 { return ARREGLO; }

"ES"                      { return ES; }
"DIAS"                    { return DIAS; }

"%"                       { return PORCENTAJE; }

"Y"                       { return Y; }
"O"                       { return O; }
"NO"                      { return NO; }

"=="                      { return IGUAL; }
"!="                      { return DIFERENTE; }
">="                      { return MAYOR_IGUAL; }
"<="                      { return MENOR_IGUAL; }
">"                       { return MAYOR; }
"<"                       { return MENOR; }

"+"                       { return MAS; }
"-"                       { return MENOS; }
"*"                       { return POR; }
"/"                       { return DIV; }

"="                       { return ASIGNAR; }
";"                       { return PUNTO_COMA; }
","                       { return COMA; }
"("                       { return PARENTESIS_IZQ; }
")"                       { return PARENTESIS_DER; }
"["                       { return CORCHETE_IZQ; }
"]"                       { return CORCHETE_DER; }

[0-9]+(\.[0-9]+)?         { yylval.valor_real = atof(yytext); return NUMERO; }

\"[^"]*\"                 { yylval.cadena = strdup(yytext); return CADENA; }

[a-zA-Z_][a-zA-Z0-9_]*    { yylval.cadena = strdup(yytext); return IDENTIFICADOR; }

.                         { fprintf(stderr, "Error léxico en línea %d: carácter inesperado '%s'\n", yylineno, yytext); exit(1); }

%%

int main() {
    yylex()
    return 0;
}

int yywrap() {
    return 1;
}