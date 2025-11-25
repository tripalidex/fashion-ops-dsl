%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int yylineno;
extern FILE *yyin;
void yyerror(const char *s);

// Enumeración para tipos
typedef enum { T_UNKNOWN, T_NUMBER, T_STRING, T_ARRAY } Type;

// Estructura para expresiones (para verificación de tipos)
typedef struct {
    Type type;
    double num_val;
    char *str_val;
} Expr;

// Estructura para símbolos
typedef struct Symbol {
    char *name;
    Type type;
    int is_const;
    int scope;
    Type array_elem_type;  // Para arreglos, el tipo de elementos
    struct Symbol *next;
} Symbol;

// Tabla de símbolos (lista enlazada simple por simplicidad, puede ser hash más adelante)
Symbol *sym_table = NULL;
int current_scope = 0;

// Funciones para tabla de símbolos
Symbol* lookup(char *name) {
    Symbol *p = sym_table;
    int max_scope = -1;
    Symbol *found = NULL;
    while (p != NULL) {
        if (strcmp(p->name, name) == 0 && p->scope <= current_scope) {
            if (p->scope > max_scope) {
                max_scope = p->scope;
                found = p;
            }
        }
        p = p->next;
    }
    return found;
}

void insert(char *name, Type type, int is_const, Type array_elem_type) {
    if (lookup(name) != NULL) {
        yyerror("Error semantico: Variable ya declarada");
        exit(1);
    }
    Symbol *s = (Symbol*)malloc(sizeof(Symbol));
    s->name = strdup(name);
    s->type = type;
    s->is_const = is_const;
    s->scope = current_scope;
    s->array_elem_type = array_elem_type;
    s->next = sym_table;
    sym_table = s;
}

// Para entrar/salir de ámbitos
void enter_scope() { current_scope++; }
void exit_scope() {
    // Remover símbolos del ámbito actual
    Symbol *p = sym_table;
    while (p != NULL) {
        if (p->scope == current_scope) {
            // Remover (simplificado, en realidad necesita desenlazar)
            free(p->name);
            Symbol *temp = p;
            p = p->next;
            free(temp);
        } else {
            p = p->next;
        }
    }
    current_scope--;
}

// Catálogo de SKU simulado (para verificación de existencia)
const char *sku_catalog[] = {"camisa_M", "pantalon_azul", "zapato_42", NULL};  // Agregar más según sea necesario
int is_sku(char *id) {
    for (int i = 0; sku_catalog[i] != NULL; i++) {
        if (strcmp(sku_catalog[i], id) == 0) return 1;
    }
    return 0;
}

%}

%union {
    double valor_real;
    char *cadena;
    Expr expr;
    Type type;  // Para tipos de lista en arreglos
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

%type <expr> expresion termino factor acceso_arreglo
%type <type> lista_expresiones  // Retorna el tipo homogéneo de la lista

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
      VARIABLE IDENTIFICADOR ASIGNAR expresion
        {
            insert($2, $4.type, 0, T_UNKNOWN);
            printf("Declaración VARIABLE: %s de tipo %d\n", $2, $4.type);
            free($2);
        }
    | CONSTANTE IDENTIFICADOR ASIGNAR expresion
        {
            insert($2, $4.type, 1, T_UNKNOWN);
            printf("Declaración CONSTANTE: %s de tipo %d\n", $2, $4.type);
            free($2);
        }
    | ARREGLO IDENTIFICADOR ASIGNAR CORCHETE_IZQ lista_expresiones CORCHETE_DER
        {
            if ($5 == T_UNKNOWN) {
                yyerror("Error semantico: Arreglo vacio, tipo indefinido");
            }
            insert($2, T_ARRAY, 0, $5);
            printf("Declaración ARREGLO: %s de elementos tipo %d\n", $2, $5);
            free($2);
        }
    ;

asignacion:
    IDENTIFICADOR ASIGNAR expresion
        {
            Symbol *s = lookup($1);
            if (s == NULL) {
                yyerror("Error semantico: Variable no declarada");
            } else if (s->is_const) {
                yyerror("Error semantico: Asignacion a constante");
            } else if (s->type != $3.type) {
                yyerror("Error semantico: Tipos incompatibles en asignacion");
            }
            printf("Asignación: %s = ...\n", $1);
            free($1);
        }
    ;

reporte:
      REPORTAR VENTAS DIARIO { printf("Reporte de ventas DIARIO\n"); }
    | REPORTAR VENTAS MENSUAL { printf("Reporte de ventas MENSUAL\n"); }
    | REPORTAR VENTAS DIARIO FILTRAR condicion { printf("Reporte de ventas DIARIO con filtro\n"); }
    | REPORTAR VENTAS MENSUAL FILTRAR condicion { printf("Reporte de ventas MENSUAL con filtro\n"); }
    ;

ver_stock:
    VER_STOCK IDENTIFICADOR ES expresion
        {
            if (!is_sku($2)) {
                yyerror("Error semantico: SKU no existe en catalogo");
            }
            if ($4.type != T_NUMBER) {
                yyerror("Error semantico: Expresion en VER_STOCK debe ser numerica");
            }
            printf("Consulta stock: %s = ...\n", $2);
            free($2);
        }
    ;

reponer:
    REPONER IDENTIFICADOR expresion
        {
            if (!is_sku($2)) {
                yyerror("Error semantico: SKU no existe en catalogo");
            }
            if ($3.type != T_NUMBER || $3.num_val <= 0) {
                yyerror("Error semantico: Cantidad en REPONER debe ser numero positivo");
            }
            printf("Reponer producto %s con cantidad %.2f\n", $2, $3.num_val);
            free($2);
        }
    ;

identificar_lentos:
      IDENTIFICAR_LENTOS DIAS MAYOR NUMERO { printf("Identificar lentos con más de %.0f días\n", $4); }
    | IDENTIFICAR_LENTOS DIAS MAYOR NUMERO VENTAS MENOR NUMERO { printf("Identificar lentos > %.0f días y ventas < %.0f\n", $4, $7); }
    ;

aplicar_descuento:
      APLICAR_DESCUENTO IDENTIFICADOR NUMERO PORCENTAJE
        {
            if (!is_sku($2)) {
                yyerror("Error semantico: SKU no existe en catalogo");
            }
            if ($3 <= 0 || $3 > 100) {
                yyerror("Error semantico: Porcentaje en APLICAR_DESCUENTO debe estar entre 0 y 100");
            }
            printf("Aplicar %.0f%% descuento a %s\n", $3, $2);
            free($2);
        }
    | APLICAR_DESCUENTO IDENTIFICADOR NUMERO PORCENTAJE SI condicion
        {
            if (!is_sku($2)) {
                yyerror("Error semantico: SKU no existe en catalogo");
            }
            if ($3 <= 0 || $3 > 100) {
                yyerror("Error semantico: Porcentaje en APLICAR_DESCUENTO debe estar entre 0 y 100");
            }
            printf("Aplicar %.0f%% descuento condicional a %s\n", $3, $2);
            free($2);
        }
    ;

si:
      SI condicion ENTONCES { enter_scope(); } programa FIN
        { printf("Estructura SI-ENTONCES-FIN\n"); exit_scope(); }
    | SI condicion ENTONCES { enter_scope(); } programa SINO { exit_scope(); enter_scope(); } programa FIN
        { printf("Estructura SI-ENTONCES-SINO-FIN\n"); exit_scope(); }
    ;

para:
    PARA IDENTIFICADOR EN IDENTIFICADOR HAZ { enter_scope(); } programa FIN
        {
            Symbol *arr = lookup($4);
            if (arr == NULL || arr->type != T_ARRAY) {
                yyerror("Error semantico: Iterador EN debe ser un ARREGLO");
            }
            insert($2, arr->array_elem_type, 0, T_UNKNOWN);  // Tipo de variable iteradora desde elementos del arreglo
            printf("Bucle PARA %s EN %s HAZ ... FIN\n", $2, $4);
            free($2); free($4);
            exit_scope();
        }
    ;

expresion:
      expresion MAS termino
        {
            if ($1.type != T_NUMBER || $3.type != T_NUMBER) {
                yyerror("Error semantico: Operandos en + deben ser numericos");
            }
            $$.type = T_NUMBER;
            $$.num_val = $1.num_val + $3.num_val;
            printf(" + ");
        }
    | expresion MENOS termino
        {
            if ($1.type != T_NUMBER || $3.type != T_NUMBER) {
                yyerror("Error semantico: Operandos en - deben ser numericos");
            }
            $$.type = T_NUMBER;
            $$.num_val = $1.num_val - $3.num_val;
            printf(" - ");
        }
    | termino { $$ = $1; }
    ;

termino:
      termino POR factor
        {
            if ($1.type != T_NUMBER || $3.type != T_NUMBER) {
                yyerror("Error semantico: Operandos en * deben ser numericos");
            }
            $$.type = T_NUMBER;
            $$.num_val = $1.num_val * $3.num_val;
            printf(" * ");
        }
    | termino DIV factor
        {
            if ($1.type != T_NUMBER || $3.type != T_NUMBER) {
                yyerror("Error semantico: Operandos en / deben ser numericos");
            }
            $$.type = T_NUMBER;
            $$.num_val = $1.num_val / $3.num_val;
            printf(" / ");
        }
    | factor { $$ = $1; }
    ;

factor:
      NUMERO
        {
            $$.type = T_NUMBER;
            $$.num_val = $1;
            printf("%.2f ", $1);
        }
    | CADENA
        {
            $$.type = T_STRING;
            $$.str_val = $1;
            printf("%s ", $1);
            // free($1);  // Diferir liberación si es necesario
        }
    | IDENTIFICADOR
        {
            Symbol *s = lookup($1);
            if (s == NULL) {
                yyerror("Error semantico: Identificador no declarado");
            }
            $$.type = s->type;
            printf("%s ", $1);
            free($1);
        }
    | acceso_arreglo { $$ = $1; }
    | PARENTESIS_IZQ expresion PARENTESIS_DER { $$ = $2; }
    ;

acceso_arreglo:
    IDENTIFICADOR CORCHETE_IZQ expresion CORCHETE_DER
        {
            Symbol *s = lookup($1);
            if (s == NULL || s->type != T_ARRAY) {
                yyerror("Error semantico: Acceso a no-arreglo");
            }
            if ($3.type != T_NUMBER || $3.num_val < 0) {  // Verificación de índice simplificada (asume entero no negativo)
                yyerror("Error semantico: Indice de arreglo invalido");
            }
            $$.type = s->array_elem_type;
            printf("Acceso a arreglo: %s[...]\n", $1);
            free($1);
        }
    ;

lista_expresiones:
      /* vacío */ { $$ = T_UNKNOWN; }
    | expresion { $$ = $1.type; }
    | lista_expresiones COMA expresion
        {
            if ($1 != $3.type) {
                yyerror("Error semantico: Elementos de arreglo con tipos mixtos");
            }
            $$ = $1;
        }
    ;

condicion:
      expresion IGUAL expresion
        {
            if ($1.type != $3.type) {
                yyerror("Error semantico: Tipos incompatibles en comparacion");
            }
            printf(" == ");
        }
    | expresion DIFERENTE expresion
        {
            if ($1.type != $3.type) {
                yyerror("Error semantico: Tipos incompatibles en comparacion");
            }
            printf(" != ");
        }
    | expresion MAYOR expresion
        {
            if ($1.type != $3.type || $1.type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en >");
            }
            printf(" > ");
        }
    | expresion MAYOR_IGUAL expresion
        {
            if ($1.type != $3.type || $1.type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en >=");
            }
            printf(" >= ");
        }
    | expresion MENOR expresion
        {
            if ($1.type != $3.type || $1.type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en <");
            }
            printf(" < ");
        }
    | expresion MENOR_IGUAL expresion
        {
            if ($1.type != $3.type || $1.type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en <=");
            }
            printf(" <= ");
        }
    | NO condicion { printf("NO "); }
    | condicion Y condicion { printf(" Y "); }
    | condicion O condicion { printf(" O "); }
    | PARENTESIS_IZQ condicion PARENTESIS_DER
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "%s en línea %d\n", s, yylineno);
    exit(1);
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

    printf("Iniciando análisis sintáctico y semántico de FashionOpsDSL...\n");
    if (yyparse() == 0) {
        printf("\nAnálisis completado exitosamente.\n");
    } else {
        printf("\nAnálisis fallido.\n");
    }

    if (argc > 1) fclose(yyin);

    return 0;
}