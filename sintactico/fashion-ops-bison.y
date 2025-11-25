%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int yylineno;
extern FILE *yyin;
void yyerror(const char *s);
%}

%union {
    double valor_real;
    char *cadena;
}

%token <valor_real> NUMERO
%token <cadena> CADENA IDENTIFICADOR

%token REPORTAR VENTAS DIARIO MENSUAL FILTRAR
%token VER_STOCK REPONER IDENTIFICAR_LENTOS APLICAR_DESCUENTO
%token SI ENTONCES SINO FIN PARA EN HAZ
%token VARIABLE CONSTANTE ARREGLO
%token ES DIAS PORCENTAJE
%token Y O NO
%token IGUAL DIFERENTE MAYOR_IGUAL MENOR_IGUAL MAYOR MENOR
%token MAS MENOS POR DIV
%token ASIGNAR PUNTO_COMA COMA PARENTESIS_IZQ PARENTESIS_DER CORCHETE_IZQ CORCHETE_DER

%left O
%left Y
%left IGUAL DIFERENTE MAYOR MAYOR_IGUAL MENOR MENOR_IGUAL
%left MAS MENOS
%left POR DIV
%right NO

%start programa

%%

programa: 
    /* vacío */
    | programa sentencia
    | programa si
    | programa para
    ;

sentencia:
      declaracion PUNTO_COMA
    | asignacion PUNTO_COMA
    | reporte PUNTO_COMA
    | ver_stock PUNTO_COMA
    | reponer PUNTO_COMA
    | identificar_lentos PUNTO_COMA
    | aplicar_descuento PUNTO_COMA
    ;

declaracion:
      VARIABLE IDENTIFICADOR ASIGNAR expresion          { printf("Declaración VARIABLE: %s\n", $2); free($2); }
    | CONSTANTE IDENTIFICADOR ASIGNAR expresion         { printf("Declaración CONSTANTE: %s\n", $2); free($2); }
    | ARREGLO IDENTIFICADOR ASIGNAR CORCHETE_IZQ lista_expresiones CORCHETE_DER  
        { printf("Declaración ARREGLO: %s = [...]\n", $2); free($2); }
    ;

asignacion:
    IDENTIFICADOR ASIGNAR expresion                     { printf("Asignación: %s = ...\n", $1); free($1); }
    ;

reporte:
      REPORTAR VENTAS DIARIO                              { printf("Reporte de ventas DIARIO\n"); }
    | REPORTAR VENTAS MENSUAL                             { printf("Reporte de ventas MENSUAL\n"); }
    | REPORTAR VENTAS DIARIO FILTRAR condicion         { printf("Reporte de ventas DIARIO con filtro\n"); }
    | REPORTAR VENTAS MENSUAL FILTRAR condicion         { printf("Reporte de ventas MENSUAL con filtro\n"); }
    ;

ver_stock:
    VER_STOCK IDENTIFICADOR ES expresion                { printf("Consulta stock: %s = ...\n", $2); free($2); }
    ;

reponer:
    REPONER IDENTIFICADOR expresion                     { printf("Reponer producto %s con cantidad ...\n", $2); free($2); }
    ;

identificar_lentos:
      IDENTIFICAR_LENTOS DIAS MAYOR NUMERO                          { printf("Identificar lentos con más de %.0f días\n", $4); }
    | IDENTIFICAR_LENTOS DIAS MAYOR NUMERO VENTAS MENOR NUMERO       { printf("Identificar lentos > %.0f días y ventas < %.0f\n", $4, $7); }
    ;

aplicar_descuento:
      APLICAR_DESCUENTO IDENTIFICADOR NUMERO PORCENTAJE                     { printf("Aplicar %.0f%% descuento a %s\n", $3, $2); free($2); }
    | APLICAR_DESCUENTO IDENTIFICADOR NUMERO PORCENTAJE SI condicion        { printf("Aplicar %.0f%% descuento condicional a %s\n", $3, $2); free($2); }
    ;

si:
      SI condicion ENTONCES programa FIN
        { printf("Estructura SI-ENTONCES-FIN\n"); }
    | SI condicion ENTONCES programa SINO programa FIN
        { printf("Estructura SI-ENTONCES-SINO-FIN\n"); }
    ;

para:
    PARA IDENTIFICADOR EN IDENTIFICADOR HAZ programa FIN
        { printf("Bucle PARA %s EN %s HAZ ... FIN\n", $2, $4); free($2); free($4); }
    ;

expresion:
      expresion MAS termino       { printf(" + "); }
    | expresion MENOS termino     { printf(" - "); }
    | termino
    ;

termino:
      termino POR factor          { printf(" * "); }
    | termino DIV factor          { printf(" / "); }
    | factor
    ;

factor:
      NUMERO                      { printf("%.2f ", $1); }
    | CADENA                      { printf("%s ", $1); free($1); }
    | IDENTIFICADOR               { printf("%s ", $1); free($1); }
    | acceso_arreglo
    | PARENTESIS_IZQ expresion PARENTESIS_DER
    ;

acceso_arreglo:
    IDENTIFICADOR CORCHETE_IZQ expresion CORCHETE_DER
        { printf("Acceso a arreglo: %s[...]\n", $1); free($1); }
    ;

lista_expresiones:
      /* vacío */
    | expresion
    | lista_expresiones COMA expresion
    ;

condicion:
      expresion IGUAL expresion           { printf(" == "); }
    | expresion DIFERENTE expresion       { printf(" != "); }
    | expresion MAYOR expresion           { printf(" > "); }
    | expresion MAYOR_IGUAL expresion     { printf(" >= "); }
    | expresion MENOR expresion           { printf(" < "); }
    | expresion MENOR_IGUAL expresion     { printf(" <= "); }
    | NO condicion                        { printf("NO "); }
    | condicion Y condicion              { printf(" Y "); }
    | condicion O condicion              { printf(" O "); }
    | PARENTESIS_IZQ condicion PARENTESIS_DER
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error sintáctico en línea %d: %s\n", yylineno, s);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (!f) {
            perror(argv[1]);
            return 1;
        }
        yyin = f;
    }

    printf("Iniciando análisis sintáctico de FashionOpsDSL (español)...\n");
    if (yyparse() == 0) {
        printf("\nAnálisis sintáctico completado exitosamente.\n");
    } else {
        printf("\nAnálisis sintáctico fallido.\n");
    }

    if (argc > 1) fclose(yyin);
    return 0;
}