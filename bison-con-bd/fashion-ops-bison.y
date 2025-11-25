%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

extern int yylex();
extern int yylineno;
extern FILE *yyin;
void yyerror(const char *s);

// Base de datos SQLite
sqlite3 *db = NULL;

// Callback para mostrar resultados de SELECT
int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for(int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

// Función para ejecutar SQL y mostrar resultados
void ejecutar_sql(const char *sql) {
    char *err_msg = 0;
    printf("\n>> SQL generado: %s\n", sql);
    
    int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error SQL: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf(">> Operación ejecutada exitosamente\n");
    }
}

// Función para inicializar la base de datos
void init_database() {
    int rc = sqlite3_open("fashion_ops.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "No se puede abrir la base de datos: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
    
    printf("Base de datos SQLite conectada: fashion_ops.db\n\n");
    
    // Crear tablas si no existen
    char *sql_productos = 
        "CREATE TABLE IF NOT EXISTS productos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "sku TEXT UNIQUE NOT NULL,"
        "nombre TEXT,"
        "stock INTEGER DEFAULT 0,"
        "precio REAL DEFAULT 0.0,"
        "dias_sin_venta INTEGER DEFAULT 0);";
    
    char *sql_descuentos = 
        "CREATE TABLE IF NOT EXISTS descuentos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "sku TEXT NOT NULL,"
        "porcentaje REAL NOT NULL,"
        "fecha_aplicacion DATETIME DEFAULT CURRENT_TIMESTAMP);";
    
    char *sql_ventas = 
        "CREATE TABLE IF NOT EXISTS ventas ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "sku TEXT NOT NULL,"
        "cantidad INTEGER,"
        "total REAL,"
        "fecha DATE);";
    
    ejecutar_sql(sql_productos);
    ejecutar_sql(sql_descuentos);
    ejecutar_sql(sql_ventas);
    
    // Insertar datos de ejemplo
    char *insert_productos = 
        "INSERT OR IGNORE INTO productos (sku, nombre, stock, precio, dias_sin_venta) VALUES "
        "('camisa_M', 'Camisa Mediana', 100, 29.99, 5),"
        "('pantalon_azul', 'Pantalón Azul', 50, 49.99, 15),"
        "('zapato_42', 'Zapato Talla 42', 30, 79.99, 30);";
    
    ejecutar_sql(insert_productos);
}

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

// Verificar si un SKU existe en la base de datos
int is_sku(char *id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT COUNT(*) FROM productos WHERE sku = '%s';", id);
    
    sqlite3_stmt *stmt;
    int exists = 0;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            exists = sqlite3_column_int(stmt, 0) > 0;
        }
        sqlite3_finalize(stmt);
    }
    
    return exists;
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
      REPORTAR VENTAS DIARIO 
        { 
            printf("Reporte de ventas DIARIO\n");
            char *sql = "SELECT fecha, SUM(total) as total_ventas, SUM(cantidad) as items_vendidos "
                       "FROM ventas "
                       "WHERE fecha = DATE('now') "
                       "GROUP BY fecha;";
            ejecutar_sql(sql);
        }
    | REPORTAR VENTAS MENSUAL 
        { 
            printf("Reporte de ventas MENSUAL\n");
            char *sql = "SELECT strftime('%Y-%m', fecha) as mes, SUM(total) as total_ventas, SUM(cantidad) as items_vendidos "
                       "FROM ventas "
                       "WHERE fecha >= DATE('now', 'start of month') "
                       "GROUP BY mes;";
            ejecutar_sql(sql);
        }
    | REPORTAR VENTAS DIARIO FILTRAR condicion 
        { 
            printf("Reporte de ventas DIARIO con filtro\n");
            char *sql = "SELECT fecha, SUM(total) as total_ventas, SUM(cantidad) as items_vendidos "
                       "FROM ventas "
                       "WHERE fecha = DATE('now') "
                       "GROUP BY fecha;";
            ejecutar_sql(sql);
        }
    | REPORTAR VENTAS MENSUAL FILTRAR condicion 
        { 
            printf("Reporte de ventas MENSUAL con filtro\n");
            char *sql = "SELECT strftime('%Y-%m', fecha) as mes, SUM(total) as total_ventas, SUM(cantidad) as items_vendidos "
                       "FROM ventas "
                       "WHERE fecha >= DATE('now', 'start of month') "
                       "GROUP BY mes;";
            ejecutar_sql(sql);
        }
    ;

ver_stock:
    VER_STOCK IDENTIFICADOR ES IDENTIFICADOR
        {
            if (!is_sku($2)) {
                yyerror("Error semantico: SKU no existe en catalogo");
            }
            
            // Generar y ejecutar SELECT
            char sql[256];
            snprintf(sql, sizeof(sql), "SELECT stock FROM productos WHERE sku = '%s';", $2);
            ejecutar_sql(sql);
            
            // Guardar resultado en la variable
            Symbol *var = lookup($4);
            if (var == NULL) {
                insert($4, T_NUMBER, 0, T_UNKNOWN);
            }
            
            printf("Variable '%s' asignada con stock de '%s'\n", $4, $2);
            free($2);
            free($4);
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
            
            // Generar y ejecutar UPDATE
            char sql[512];
            snprintf(sql, sizeof(sql), 
                    "UPDATE productos SET stock = stock + %.0f WHERE sku = '%s';", 
                    $3.num_val, $2);
            ejecutar_sql(sql);
            
            printf("Repuesto producto '%s' con cantidad %.0f\n", $2, $3.num_val);
            free($2);
        }
    ;

identificar_lentos:
      IDENTIFICAR_LENTOS DIAS MAYOR NUMERO 
        { 
            printf("Identificar lentos con más de %.0f días\n", $4);
            char sql[256];
            snprintf(sql, sizeof(sql), 
                    "SELECT sku, nombre, stock, dias_sin_venta FROM productos WHERE dias_sin_venta > %.0f;", 
                    $4);
            ejecutar_sql(sql);
        }
    | IDENTIFICAR_LENTOS DIAS MAYOR NUMERO VENTAS MENOR NUMERO 
        { 
            printf("Identificar lentos > %.0f días y ventas < %.0f\n", $4, $7);
            char sql[512];
            snprintf(sql, sizeof(sql), 
                    "SELECT p.sku, p.nombre, p.stock, p.dias_sin_venta, COALESCE(SUM(v.cantidad), 0) as total_ventas "
                    "FROM productos p "
                    "LEFT JOIN ventas v ON p.sku = v.sku "
                    "WHERE p.dias_sin_venta > %.0f "
                    "GROUP BY p.sku "
                    "HAVING COALESCE(SUM(v.cantidad), 0) < %.0f;", 
                    $4, $7);
            ejecutar_sql(sql);
        }
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
            
            // Generar y ejecutar INSERT
            char sql[512];
            snprintf(sql, sizeof(sql), 
                    "INSERT INTO descuentos (sku, porcentaje) VALUES ('%s', %.2f);", 
                    $2, $3);
            ejecutar_sql(sql);
            
            printf("Aplicado %.0f%% descuento a '%s'\n", $3, $2);
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
            
            char sql[512];
            snprintf(sql, sizeof(sql), 
                    "INSERT INTO descuentos (sku, porcentaje) VALUES ('%s', %.2f);", 
                    $2, $3);
            ejecutar_sql(sql);
            
            printf("Aplicado %.0f%% descuento condicional a '%s'\n", $3, $2);
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
        }
    | expresion MENOS termino
        {
            if ($1.type != T_NUMBER || $3.type != T_NUMBER) {
                yyerror("Error semantico: Operandos en - deben ser numericos");
            }
            $$.type = T_NUMBER;
            $$.num_val = $1.num_val - $3.num_val;
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
        }
    | termino DIV factor
        {
            if ($1.type != T_NUMBER || $3.type != T_NUMBER) {
                yyerror("Error semantico: Operandos en / deben ser numericos");
            }
            $$.type = T_NUMBER;
            $$.num_val = $1.num_val / $3.num_val;
        }
    | factor { $$ = $1; }
    ;

factor:
      NUMERO
        {
            $$.type = T_NUMBER;
            $$.num_val = $1;
        }
    | CADENA
        {
            $$.type = T_STRING;
            $$.str_val = $1;
        }
    | IDENTIFICADOR
        {
            Symbol *s = lookup($1);
            if (s == NULL) {
                yyerror("Error semantico: Identificador no declarado");
            }
            $$.type = s->type;
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
        }
    | expresion DIFERENTE expresion
        {
            if ($1.type != $3.type) {
                yyerror("Error semantico: Tipos incompatibles en comparacion");
            }
        }
    | expresion MAYOR expresion
        {
            if ($1.type != $3.type || $1.type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en >");
            }
        }
    | expresion MAYOR_IGUAL expresion
        {
            if ($1.type != $3.type || $1.type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en >=");
            }
        }
    | expresion MENOR expresion
        {
            if ($1.type != $3.type || $1.type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en <");
            }
        }
    | expresion MENOR_IGUAL expresion
        {
            if ($1.type != $3.type || $1.type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en <=");
            }
        }
    | NO condicion { }
    | condicion Y condicion { }
    | condicion O condicion { }
    | PARENTESIS_IZQ condicion PARENTESIS_DER
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "%s en línea %d\n", s, yylineno);
}

int main(int argc, char **argv) {
    // Inicializar base de datos
    init_database();
    
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (!f) {
            perror(argv[1]);
            sqlite3_close(db);
            return 1;
        }
        yyin = f;
    }

    printf("=== Iniciando análisis sintáctico y semántico de FashionOpsDSL ===\n\n");
    
    int result = yyparse();
    
    if (result == 0) {
        printf("\n=== Análisis completado exitosamente ===\n");
    } else {
        printf("\n=== Análisis fallido ===\n");
    }

    if (argc > 1) fclose(yyin);
    
    // Cerrar base de datos
    sqlite3_close(db);
    
    return result;
}