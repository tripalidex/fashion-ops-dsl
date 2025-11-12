%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declaraciones Flex
extern int yylex();
extern int yylineno;
extern FILE *yyin;

void yyerror(const char *s);

// Estructuras de datos para semántico
union YYSTYPE {
    float fval;   // Para números
    char *sval;   // Para IDs y STRINGS
    // Puedes añadir más campos aquí para nodos AST, etc.
};
#define YYSTYPE union YYSTYPE
%}

// Declaración de terminales
%token REPORT STOCK REORDER SLOW DISCOUNT IF THEN ELSE FOR IN VAR CONST ARRAY END
%token SALES DAILY MONTHLY FILTER IS DAYS PERCENT DO
%token EQ NEQ GEQ LEQ GT LT PLUS MINUS MULT DIV AND OR NOT ASSIGN
%token SEMICOLON LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE COMMA
%token NUMBER STRING ID

// Definición de la precedencia y asociatividad de operadores
// Esto ayuda a resolver ambigüedades en la gramática y construye expresiones correctamente
%left OR
%left AND
%left EQ NEQ GT GEQ LT LEQ
%left PLUS MINUS
%left MULT DIV
%right NOT // Unario NOT
%precedence UMINUS // Precedencia para el menos unario (puedes no necesitarlo si tu gramática maneja bien esto)

// Definición del símbolo inicial de la gramática
%start program

%%

// Reglas de Producción
program:
    /* empty */
    | program statement SEMICOLON
    | program if_stmt
    | program for_stmt
;

statement:
    declaration
    | assignment
    | report_stmt
    | stock_stmt
    | reorder_stmt
    | slow_stmt
    | discount_stmt
;

declaration:
    VAR ID ASSIGN expr
    { printf("Declaracion VAR: %s = ...\n", $2); }
    | CONST ID ASSIGN expr
    { printf("Declaracion CONST: %s = ...\n", $2); }
    | ARRAY ID ASSIGN LBRACKET expr_list RBRACKET
    { printf("Declaracion ARRAY: %s = [...] (con %d elementos)\n", $2, yyleng); /* yyleng aquí no es exacto, solo un marcador */ }
;

assignment:
    ID ASSIGN expr
    { printf("Asignacion: %s = ...\n", $1); }
;

report_stmt:
    REPORT SALES DAILY
    { printf("Reporte de Ventas Diario\n"); }
    | REPORT SALES MONTHLY
    { printf("Reporte de Ventas Mensual\n"); }
    | REPORT SALES DAILY FILTER condition
    { printf("Reporte de Ventas Diario con Filtro\n"); }
    | REPORT SALES MONTHLY FILTER condition
    { printf("Reporte de Ventas Mensual con Filtro\n"); }
;

stock_stmt:
    STOCK ID IS expr
    { printf("Consulta de Stock: %s es %f\n", $2, $4.fval); }
;

reorder_stmt:
    REORDER ID NUMBER
    { printf("Orden de Reabastecimiento: %s, cantidad: %f\n", $2, $3.fval); }
;

slow_stmt:
    SLOW IN ID DAYS GT NUMBER
    { printf("Identificar Slow Movers en %s, mas de %f dias\n", $3, $6.fval); }
    | SLOW IN ID DAYS GT NUMBER SALES LT NUMBER
    { printf("Identificar Slow Movers en %s, mas de %f dias, ventas menores a %f\n", $3, $6.fval, $9.fval); }
;

discount_stmt:
    DISCOUNT ID NUMBER PERCENT
    { printf("Descuento para %s: %f%%\n", $2, $3.fval); }
    | DISCOUNT ID NUMBER PERCENT IF condition
    { printf("Descuento condicional para %s: %f%%\n", $2, $3.fval); }
;

if_stmt:
    IF condition THEN program ELSE program END
    { printf("Estructura IF-THEN-ELSE-END\n"); }
    | IF condition THEN program END
    { printf("Estructura IF-THEN-END\n"); }
;

for_stmt:
    FOR ID IN ID DO program END
    { printf("Bucle FOR: iterando %s en %s\n", $2, $4); }
;

expr:
    term
    | expr PLUS term      { $$ = $1; printf(" + "); } // Simples acciones para mostrar la construcción
    | expr MINUS term     { $$ = $1; printf(" - "); }
;

term:
    factor
    | term MULT factor    { $$ = $1; printf(" * "); }
    | term DIV factor     { $$ = $1; printf(" / "); }
;

factor:
    NUMBER                { $$ = $1; printf("%f ", $1.fval); }
    | STRING              { $$ = $1; printf("\"%s\" ", $1); }
    | ID                  { $$ = $1; printf("%s ", $1); }
    | array_access
    | LPAREN expr RPAREN
;

array_access:
    ID LBRACKET expr RBRACKET
    { printf("Acceso a arreglo: %s[%s]\n", $1, "indice"); } // Solo marcador por ahora
;

expr_list:
    /* empty */
    | expr
    | expr_list COMMA expr
;

condition:
    expr EQ expr          { printf(" == "); }
    | expr NEQ expr       { printf(" != "); }
    | expr GT expr        { printf(" > "); }
    | expr GEQ expr       { printf(" >= "); }
    | expr LT expr        { printf(" < "); }
    | expr LEQ expr       { printf(" <= "); }
    | NOT condition
    | condition AND condition
    | condition OR condition
    | LPAREN condition RPAREN
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error sintáctico en la línea %d: %s\n", yylineno, s);
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

    printf("Iniciando análisis sintáctico...\n");
    if (yyparse() == 0) {
        printf("Análisis sintáctico completado exitosamente.\n");
    } else {
        printf("Análisis sintáctico fallido.\n");
    }

    if (argc > 1) {
        fclose(yyin);
    }

    return 0;
}