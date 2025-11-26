/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30704

/* Bison version string.  */
#define YYBISON_VERSION "3.7.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "fashion-ops-bison.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "types.h"

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


#line 245 "fashion-ops-bison.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "fashion-ops-bison.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUMERO = 3,                     /* NUMERO  */
  YYSYMBOL_CADENA = 4,                     /* CADENA  */
  YYSYMBOL_IDENTIFICADOR = 5,              /* IDENTIFICADOR  */
  YYSYMBOL_REPORTAR = 6,                   /* REPORTAR  */
  YYSYMBOL_VENTAS = 7,                     /* VENTAS  */
  YYSYMBOL_DIARIO = 8,                     /* DIARIO  */
  YYSYMBOL_MENSUAL = 9,                    /* MENSUAL  */
  YYSYMBOL_FILTRAR = 10,                   /* FILTRAR  */
  YYSYMBOL_VER_STOCK = 11,                 /* VER_STOCK  */
  YYSYMBOL_REPONER = 12,                   /* REPONER  */
  YYSYMBOL_IDENTIFICAR_LENTOS = 13,        /* IDENTIFICAR_LENTOS  */
  YYSYMBOL_APLICAR_DESCUENTO = 14,         /* APLICAR_DESCUENTO  */
  YYSYMBOL_SI = 15,                        /* SI  */
  YYSYMBOL_ENTONCES = 16,                  /* ENTONCES  */
  YYSYMBOL_SINO = 17,                      /* SINO  */
  YYSYMBOL_FIN = 18,                       /* FIN  */
  YYSYMBOL_PARA = 19,                      /* PARA  */
  YYSYMBOL_EN = 20,                        /* EN  */
  YYSYMBOL_HAZ = 21,                       /* HAZ  */
  YYSYMBOL_VARIABLE = 22,                  /* VARIABLE  */
  YYSYMBOL_CONSTANTE = 23,                 /* CONSTANTE  */
  YYSYMBOL_ARREGLO = 24,                   /* ARREGLO  */
  YYSYMBOL_ES = 25,                        /* ES  */
  YYSYMBOL_DIAS = 26,                      /* DIAS  */
  YYSYMBOL_PORCENTAJE = 27,                /* PORCENTAJE  */
  YYSYMBOL_Y = 28,                         /* Y  */
  YYSYMBOL_O = 29,                         /* O  */
  YYSYMBOL_NO = 30,                        /* NO  */
  YYSYMBOL_IGUAL = 31,                     /* IGUAL  */
  YYSYMBOL_DIFERENTE = 32,                 /* DIFERENTE  */
  YYSYMBOL_MAYOR_IGUAL = 33,               /* MAYOR_IGUAL  */
  YYSYMBOL_MENOR_IGUAL = 34,               /* MENOR_IGUAL  */
  YYSYMBOL_MAYOR = 35,                     /* MAYOR  */
  YYSYMBOL_MENOR = 36,                     /* MENOR  */
  YYSYMBOL_MAS = 37,                       /* MAS  */
  YYSYMBOL_MENOS = 38,                     /* MENOS  */
  YYSYMBOL_POR = 39,                       /* POR  */
  YYSYMBOL_DIV = 40,                       /* DIV  */
  YYSYMBOL_ASIGNAR = 41,                   /* ASIGNAR  */
  YYSYMBOL_PUNTO_COMA = 42,                /* PUNTO_COMA  */
  YYSYMBOL_COMA = 43,                      /* COMA  */
  YYSYMBOL_PARENTESIS_IZQ = 44,            /* PARENTESIS_IZQ  */
  YYSYMBOL_PARENTESIS_DER = 45,            /* PARENTESIS_DER  */
  YYSYMBOL_CORCHETE_IZQ = 46,              /* CORCHETE_IZQ  */
  YYSYMBOL_CORCHETE_DER = 47,              /* CORCHETE_DER  */
  YYSYMBOL_YYACCEPT = 48,                  /* $accept  */
  YYSYMBOL_programa = 49,                  /* programa  */
  YYSYMBOL_sentencia = 50,                 /* sentencia  */
  YYSYMBOL_declaracion = 51,               /* declaracion  */
  YYSYMBOL_asignacion = 52,                /* asignacion  */
  YYSYMBOL_reporte = 53,                   /* reporte  */
  YYSYMBOL_ver_stock = 54,                 /* ver_stock  */
  YYSYMBOL_reponer = 55,                   /* reponer  */
  YYSYMBOL_identificar_lentos = 56,        /* identificar_lentos  */
  YYSYMBOL_aplicar_descuento = 57,         /* aplicar_descuento  */
  YYSYMBOL_si = 58,                        /* si  */
  YYSYMBOL_59_1 = 59,                      /* $@1  */
  YYSYMBOL_60_2 = 60,                      /* $@2  */
  YYSYMBOL_61_3 = 61,                      /* $@3  */
  YYSYMBOL_para = 62,                      /* para  */
  YYSYMBOL_63_4 = 63,                      /* $@4  */
  YYSYMBOL_expresion = 64,                 /* expresion  */
  YYSYMBOL_termino = 65,                   /* termino  */
  YYSYMBOL_factor = 66,                    /* factor  */
  YYSYMBOL_acceso_arreglo = 67,            /* acceso_arreglo  */
  YYSYMBOL_lista_expresiones = 68,         /* lista_expresiones  */
  YYSYMBOL_condicion = 69                  /* condicion  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   176

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  48
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  58
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  131

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   302


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   208,   208,   210,   211,   212,   216,   217,   218,   219,
     220,   221,   222,   226,   232,   238,   250,   266,   275,   284,
     293,   305,   329,   351,   360,   377,   396,   417,   417,   419,
     419,   419,   424,   424,   438,   446,   454,   458,   466,   474,
     478,   483,   488,   497,   498,   502,   517,   518,   519,   529,
     535,   541,   547,   553,   559,   565,   566,   567,   568
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUMERO", "CADENA",
  "IDENTIFICADOR", "REPORTAR", "VENTAS", "DIARIO", "MENSUAL", "FILTRAR",
  "VER_STOCK", "REPONER", "IDENTIFICAR_LENTOS", "APLICAR_DESCUENTO", "SI",
  "ENTONCES", "SINO", "FIN", "PARA", "EN", "HAZ", "VARIABLE", "CONSTANTE",
  "ARREGLO", "ES", "DIAS", "PORCENTAJE", "Y", "O", "NO", "IGUAL",
  "DIFERENTE", "MAYOR_IGUAL", "MENOR_IGUAL", "MAYOR", "MENOR", "MAS",
  "MENOS", "POR", "DIV", "ASIGNAR", "PUNTO_COMA", "COMA", "PARENTESIS_IZQ",
  "PARENTESIS_DER", "CORCHETE_IZQ", "CORCHETE_DER", "$accept", "programa",
  "sentencia", "declaracion", "asignacion", "reporte", "ver_stock",
  "reponer", "identificar_lentos", "aplicar_descuento", "si", "$@1", "$@2",
  "$@3", "para", "$@4", "expresion", "termino", "factor", "acceso_arreglo",
  "lista_expresiones", "condicion", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302
};
#endif

#define YYPACT_NINF (-76)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-30)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -76,    57,   -76,   -18,     7,    15,    25,    40,    29,     1,
      42,    46,    62,    70,   -76,    35,    36,    41,    43,    47,
      48,    49,   -76,   -76,     8,    24,    63,     8,    71,    94,
     -76,   -76,    66,     1,     1,   135,    -2,   -76,   -76,    -7,
     101,    73,    86,    87,   -76,   -76,   -76,   -76,   -76,   -76,
     -76,     8,    21,   132,   133,   131,    21,   143,   134,     8,
     -76,   120,   -27,     8,     8,     8,     8,     8,     8,     8,
       8,     8,     8,   130,     1,     1,   154,     8,     8,   114,
      -9,     1,     1,   -76,   155,   148,   -21,   -76,   -76,    21,
      21,    21,    21,    21,    21,    -2,    -2,   -76,   -76,   -76,
     -76,   -76,   136,   153,    21,    21,     8,    32,    32,   139,
       1,   -76,    81,    96,   -76,    21,   -28,   173,    32,   -76,
     -76,   -76,     8,   -76,   -76,   -76,   111,    21,   126,   -76,
     -76
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     3,     0,     0,     0,     0,     0,
       0,     0,     4,     5,     0,     0,     0,     0,     0,     0,
      40,    41,    42,     0,     0,     0,    36,    39,    43,     0,
       0,     0,     0,     0,     6,     7,     8,     9,    10,    11,
      12,     0,    16,    17,    18,     0,    22,     0,     0,     0,
      55,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    21,    23,    25,     0,    44,    58,    49,
      50,    52,    54,    51,    53,    34,    35,    37,    38,     2,
       2,    56,    57,     0,    13,    14,    46,    19,    20,     0,
       0,    45,     0,     0,    32,    47,     0,     0,    26,    28,
      30,     2,     0,    15,    24,     2,     0,    48,     0,    33,
      31
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -76,   -75,   -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,
     -76,   -76,   -76,   -76,   -76,   -76,   -24,    -5,     2,   -76,
     -76,   -26
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    99,   100,   125,    23,   121,    35,    36,    37,    38,
     116,    39
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      52,    74,    75,    56,    30,    31,    32,    60,    62,    73,
      61,    30,    31,    32,    25,   122,    69,    70,    88,   123,
      26,    74,    75,    24,   112,   113,   111,    80,    69,    70,
      27,    33,    53,    54,    29,    86,    87,    71,    72,    89,
      90,    91,    92,    93,    94,    34,   126,    40,   101,   102,
     128,    41,    51,   104,   105,   107,   108,     2,    69,    70,
      74,    75,     3,     4,    95,    96,    28,    42,     5,     6,
       7,     8,     9,    97,    98,    43,    10,    44,    45,    11,
      12,    13,   115,    46,   118,    47,     3,     4,    55,    48,
      49,    50,     5,     6,     7,     8,     9,    58,   127,   119,
      10,     3,     4,    11,    12,    13,    57,     5,     6,     7,
       8,     9,    59,   120,    77,    10,     3,     4,    11,    12,
      13,    76,     5,     6,     7,     8,     9,    78,    79,   129,
      10,     3,     4,    11,    12,    13,    83,     5,     6,     7,
       8,     9,    81,    82,   130,    10,    84,   -29,    11,    12,
      13,    63,    64,    65,    66,    67,    68,    69,    70,   103,
     106,    85,   109,   110,    74,    87,    63,    64,    65,    66,
      67,    68,    69,    70,   114,   117,   124
};

static const yytype_int8 yycheck[] =
{
      24,    28,    29,    27,     3,     4,     5,    33,    34,    16,
      34,     3,     4,     5,     7,    43,    37,    38,    45,    47,
       5,    28,    29,    41,    99,   100,    47,    51,    37,    38,
       5,    30,     8,     9,     5,    59,    45,    39,    40,    63,
      64,    65,    66,    67,    68,    44,   121,     5,    74,    75,
     125,     5,    44,    77,    78,    81,    82,     0,    37,    38,
      28,    29,     5,     6,    69,    70,    26,     5,    11,    12,
      13,    14,    15,    71,    72,     5,    19,    42,    42,    22,
      23,    24,   106,    42,   110,    42,     5,     6,    25,    42,
      42,    42,    11,    12,    13,    14,    15,     3,   122,    18,
      19,     5,     6,    22,    23,    24,    35,    11,    12,    13,
      14,    15,    46,    17,    41,    19,     5,     6,    22,    23,
      24,    20,    11,    12,    13,    14,    15,    41,    41,    18,
      19,     5,     6,    22,    23,    24,     5,    11,    12,    13,
      14,    15,    10,    10,    18,    19,     3,    17,    22,    23,
      24,    31,    32,    33,    34,    35,    36,    37,    38,     5,
      46,    27,     7,    15,    28,    45,    31,    32,    33,    34,
      35,    36,    37,    38,    21,    36,     3
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    49,     0,     5,     6,    11,    12,    13,    14,    15,
      19,    22,    23,    24,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    62,    41,     7,     5,     5,    26,     5,
       3,     4,     5,    30,    44,    64,    65,    66,    67,    69,
       5,     5,     5,     5,    42,    42,    42,    42,    42,    42,
      42,    44,    64,     8,     9,    25,    64,    35,     3,    46,
      69,    64,    69,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    16,    28,    29,    20,    41,    41,    41,
      64,    10,    10,     5,     3,    27,    64,    45,    45,    64,
      64,    64,    64,    64,    64,    65,    65,    66,    66,    59,
      60,    69,    69,     5,    64,    64,    46,    69,    69,     7,
      15,    47,    49,    49,    21,    64,    68,    36,    69,    18,
      17,    63,    43,    47,     3,    61,    49,    64,    49,    18,
      18
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    48,    49,    49,    49,    49,    50,    50,    50,    50,
      50,    50,    50,    51,    51,    51,    52,    53,    53,    53,
      53,    54,    55,    56,    56,    57,    57,    59,    58,    60,
      61,    58,    63,    62,    64,    64,    64,    65,    65,    65,
      66,    66,    66,    66,    66,    67,    68,    68,    68,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     4,     4,     6,     3,     3,     3,     5,
       5,     4,     3,     4,     7,     4,     6,     0,     6,     0,
       0,     9,     0,     8,     3,     3,     1,     3,     3,     1,
       1,     1,     1,     1,     3,     4,     0,     1,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     3,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 13: /* declaracion: VARIABLE IDENTIFICADOR ASIGNAR expresion  */
#line 227 "fashion-ops-bison.y"
        {
            insert((yyvsp[-2].cadena), (yyvsp[0].expr).type, 0, T_UNKNOWN);
            printf("Declaración VARIABLE: %s de tipo %d\n", (yyvsp[-2].cadena), (yyvsp[0].expr).type);
            free((yyvsp[-2].cadena));
        }
#line 1397 "fashion-ops-bison.tab.c"
    break;

  case 14: /* declaracion: CONSTANTE IDENTIFICADOR ASIGNAR expresion  */
#line 233 "fashion-ops-bison.y"
        {
            insert((yyvsp[-2].cadena), (yyvsp[0].expr).type, 1, T_UNKNOWN);
            printf("Declaración CONSTANTE: %s de tipo %d\n", (yyvsp[-2].cadena), (yyvsp[0].expr).type);
            free((yyvsp[-2].cadena));
        }
#line 1407 "fashion-ops-bison.tab.c"
    break;

  case 15: /* declaracion: ARREGLO IDENTIFICADOR ASIGNAR CORCHETE_IZQ lista_expresiones CORCHETE_DER  */
#line 239 "fashion-ops-bison.y"
        {
            if ((yyvsp[-1].type) == T_UNKNOWN) {
                yyerror("Error semantico: Arreglo vacio, tipo indefinido");
            }
            insert((yyvsp[-4].cadena), T_ARRAY, 0, (yyvsp[-1].type));
            printf("Declaración ARREGLO: %s de elementos tipo %d\n", (yyvsp[-4].cadena), (yyvsp[-1].type));
            free((yyvsp[-4].cadena));
        }
#line 1420 "fashion-ops-bison.tab.c"
    break;

  case 16: /* asignacion: IDENTIFICADOR ASIGNAR expresion  */
#line 251 "fashion-ops-bison.y"
        {
            Symbol *s = lookup((yyvsp[-2].cadena));
            if (s == NULL) {
                yyerror("Error semantico: Variable no declarada");
            } else if (s->is_const) {
                yyerror("Error semantico: Asignacion a constante");
            } else if (s->type != (yyvsp[0].expr).type) {
                yyerror("Error semantico: Tipos incompatibles en asignacion");
            }
            printf("Asignación: %s = ...\n", (yyvsp[-2].cadena));
            free((yyvsp[-2].cadena));
        }
#line 1437 "fashion-ops-bison.tab.c"
    break;

  case 17: /* reporte: REPORTAR VENTAS DIARIO  */
#line 267 "fashion-ops-bison.y"
        { 
            printf("Reporte de ventas DIARIO\n");
            char *sql = "SELECT fecha, SUM(total) as total_ventas, SUM(cantidad) as items_vendidos "
                       "FROM ventas "
                       "WHERE fecha = DATE('now') "
                       "GROUP BY fecha;";
            ejecutar_sql(sql);
        }
#line 1450 "fashion-ops-bison.tab.c"
    break;

  case 18: /* reporte: REPORTAR VENTAS MENSUAL  */
#line 276 "fashion-ops-bison.y"
        { 
            printf("Reporte de ventas MENSUAL\n");
            char *sql = "SELECT strftime('%Y-%m', fecha) as mes, SUM(total) as total_ventas, SUM(cantidad) as items_vendidos "
                       "FROM ventas "
                       "WHERE fecha >= DATE('now', 'start of month') "
                       "GROUP BY mes;";
            ejecutar_sql(sql);
        }
#line 1463 "fashion-ops-bison.tab.c"
    break;

  case 19: /* reporte: REPORTAR VENTAS DIARIO FILTRAR condicion  */
#line 285 "fashion-ops-bison.y"
        { 
            printf("Reporte de ventas DIARIO con filtro\n");
            char *sql = "SELECT fecha, SUM(total) as total_ventas, SUM(cantidad) as items_vendidos "
                       "FROM ventas "
                       "WHERE fecha = DATE('now') "
                       "GROUP BY fecha;";
            ejecutar_sql(sql);
        }
#line 1476 "fashion-ops-bison.tab.c"
    break;

  case 20: /* reporte: REPORTAR VENTAS MENSUAL FILTRAR condicion  */
#line 294 "fashion-ops-bison.y"
        { 
            printf("Reporte de ventas MENSUAL con filtro\n");
            char *sql = "SELECT strftime('%Y-%m', fecha) as mes, SUM(total) as total_ventas, SUM(cantidad) as items_vendidos "
                       "FROM ventas "
                       "WHERE fecha >= DATE('now', 'start of month') "
                       "GROUP BY mes;";
            ejecutar_sql(sql);
        }
#line 1489 "fashion-ops-bison.tab.c"
    break;

  case 21: /* ver_stock: VER_STOCK IDENTIFICADOR ES IDENTIFICADOR  */
#line 306 "fashion-ops-bison.y"
        {
            if (!is_sku((yyvsp[-2].cadena))) {
                yyerror("Error semantico: SKU no existe en catalogo");
            }
            
            // Generar y ejecutar SELECT
            char sql[256];
            snprintf(sql, sizeof(sql), "SELECT stock FROM productos WHERE sku = '%s';", (yyvsp[-2].cadena));
            ejecutar_sql(sql);
            
            // Guardar resultado en la variable
            Symbol *var = lookup((yyvsp[0].cadena));
            if (var == NULL) {
                insert((yyvsp[0].cadena), T_NUMBER, 0, T_UNKNOWN);
            }
            
            printf("Variable '%s' asignada con stock de '%s'\n", (yyvsp[0].cadena), (yyvsp[-2].cadena));
            free((yyvsp[-2].cadena));
            free((yyvsp[0].cadena));
        }
#line 1514 "fashion-ops-bison.tab.c"
    break;

  case 22: /* reponer: REPONER IDENTIFICADOR expresion  */
#line 330 "fashion-ops-bison.y"
        {
            if (!is_sku((yyvsp[-1].cadena))) {
                yyerror("Error semantico: SKU no existe en catalogo");
            }
            if ((yyvsp[0].expr).type != T_NUMBER || (yyvsp[0].expr).num_val <= 0) {
                yyerror("Error semantico: Cantidad en REPONER debe ser numero positivo");
            }
            
            // Generar y ejecutar UPDATE
            char sql[512];
            snprintf(sql, sizeof(sql), 
                    "UPDATE productos SET stock = stock + %.0f WHERE sku = '%s';", 
                    (yyvsp[0].expr).num_val, (yyvsp[-1].cadena));
            ejecutar_sql(sql);
            
            printf("Repuesto producto '%s' con cantidad %.0f\n", (yyvsp[-1].cadena), (yyvsp[0].expr).num_val);
            free((yyvsp[-1].cadena));
        }
#line 1537 "fashion-ops-bison.tab.c"
    break;

  case 23: /* identificar_lentos: IDENTIFICAR_LENTOS DIAS MAYOR NUMERO  */
#line 352 "fashion-ops-bison.y"
        { 
            printf("Identificar lentos con más de %.0f días\n", (yyvsp[0].valor_real));
            char sql[256];
            snprintf(sql, sizeof(sql), 
                    "SELECT sku, nombre, stock, dias_sin_venta FROM productos WHERE dias_sin_venta > %.0f;", 
                    (yyvsp[0].valor_real));
            ejecutar_sql(sql);
        }
#line 1550 "fashion-ops-bison.tab.c"
    break;

  case 24: /* identificar_lentos: IDENTIFICAR_LENTOS DIAS MAYOR NUMERO VENTAS MENOR NUMERO  */
#line 361 "fashion-ops-bison.y"
        { 
            printf("Identificar lentos > %.0f días y ventas < %.0f\n", (yyvsp[-3].valor_real), (yyvsp[0].valor_real));
            char sql[512];
            snprintf(sql, sizeof(sql), 
                    "SELECT p.sku, p.nombre, p.stock, p.dias_sin_venta, COALESCE(SUM(v.cantidad), 0) as total_ventas "
                    "FROM productos p "
                    "LEFT JOIN ventas v ON p.sku = v.sku "
                    "WHERE p.dias_sin_venta > %.0f "
                    "GROUP BY p.sku "
                    "HAVING COALESCE(SUM(v.cantidad), 0) < %.0f;", 
                    (yyvsp[-3].valor_real), (yyvsp[0].valor_real));
            ejecutar_sql(sql);
        }
#line 1568 "fashion-ops-bison.tab.c"
    break;

  case 25: /* aplicar_descuento: APLICAR_DESCUENTO IDENTIFICADOR NUMERO PORCENTAJE  */
#line 378 "fashion-ops-bison.y"
        {
            if (!is_sku((yyvsp[-2].cadena))) {
                yyerror("Error semantico: SKU no existe en catalogo");
            }
            if ((yyvsp[-1].valor_real) <= 0 || (yyvsp[-1].valor_real) > 100) {
                yyerror("Error semantico: Porcentaje en APLICAR_DESCUENTO debe estar entre 0 y 100");
            }
            
            // Generar y ejecutar INSERT
            char sql[512];
            snprintf(sql, sizeof(sql), 
                    "INSERT INTO descuentos (sku, porcentaje) VALUES ('%s', %.2f);", 
                    (yyvsp[-2].cadena), (yyvsp[-1].valor_real));
            ejecutar_sql(sql);
            
            printf("Aplicado %.0f%% descuento a '%s'\n", (yyvsp[-1].valor_real), (yyvsp[-2].cadena));
            free((yyvsp[-2].cadena));
        }
#line 1591 "fashion-ops-bison.tab.c"
    break;

  case 26: /* aplicar_descuento: APLICAR_DESCUENTO IDENTIFICADOR NUMERO PORCENTAJE SI condicion  */
#line 397 "fashion-ops-bison.y"
        {
            if (!is_sku((yyvsp[-4].cadena))) {
                yyerror("Error semantico: SKU no existe en catalogo");
            }
            if ((yyvsp[-3].valor_real) <= 0 || (yyvsp[-3].valor_real) > 100) {
                yyerror("Error semantico: Porcentaje en APLICAR_DESCUENTO debe estar entre 0 y 100");
            }
            
            char sql[512];
            snprintf(sql, sizeof(sql), 
                    "INSERT INTO descuentos (sku, porcentaje) VALUES ('%s', %.2f);", 
                    (yyvsp[-4].cadena), (yyvsp[-3].valor_real));
            ejecutar_sql(sql);
            
            printf("Aplicado %.0f%% descuento condicional a '%s'\n", (yyvsp[-3].valor_real), (yyvsp[-4].cadena));
            free((yyvsp[-4].cadena));
        }
#line 1613 "fashion-ops-bison.tab.c"
    break;

  case 27: /* $@1: %empty  */
#line 417 "fashion-ops-bison.y"
                            { enter_scope(); }
#line 1619 "fashion-ops-bison.tab.c"
    break;

  case 28: /* si: SI condicion ENTONCES $@1 programa FIN  */
#line 418 "fashion-ops-bison.y"
        { printf("Estructura SI-ENTONCES-FIN\n"); exit_scope(); }
#line 1625 "fashion-ops-bison.tab.c"
    break;

  case 29: /* $@2: %empty  */
#line 419 "fashion-ops-bison.y"
                            { enter_scope(); }
#line 1631 "fashion-ops-bison.tab.c"
    break;

  case 30: /* $@3: %empty  */
#line 419 "fashion-ops-bison.y"
                                                             { exit_scope(); enter_scope(); }
#line 1637 "fashion-ops-bison.tab.c"
    break;

  case 31: /* si: SI condicion ENTONCES $@2 programa SINO $@3 programa FIN  */
#line 420 "fashion-ops-bison.y"
        { printf("Estructura SI-ENTONCES-SINO-FIN\n"); exit_scope(); }
#line 1643 "fashion-ops-bison.tab.c"
    break;

  case 32: /* $@4: %empty  */
#line 424 "fashion-ops-bison.y"
                                            { enter_scope(); }
#line 1649 "fashion-ops-bison.tab.c"
    break;

  case 33: /* para: PARA IDENTIFICADOR EN IDENTIFICADOR HAZ $@4 programa FIN  */
#line 425 "fashion-ops-bison.y"
        {
            Symbol *arr = lookup((yyvsp[-4].cadena));
            if (arr == NULL || arr->type != T_ARRAY) {
                yyerror("Error semantico: Iterador EN debe ser un ARREGLO");
            }
            insert((yyvsp[-6].cadena), arr->array_elem_type, 0, T_UNKNOWN);  // Tipo de variable iteradora desde elementos del arreglo
            printf("Bucle PARA %s EN %s HAZ ... FIN\n", (yyvsp[-6].cadena), (yyvsp[-4].cadena));
            free((yyvsp[-6].cadena)); free((yyvsp[-4].cadena));
            exit_scope();
        }
#line 1664 "fashion-ops-bison.tab.c"
    break;

  case 34: /* expresion: expresion MAS termino  */
#line 439 "fashion-ops-bison.y"
        {
            if ((yyvsp[-2].expr).type != T_NUMBER || (yyvsp[0].expr).type != T_NUMBER) {
                yyerror("Error semantico: Operandos en + deben ser numericos");
            }
            (yyval.expr).type = T_NUMBER;
            (yyval.expr).num_val = (yyvsp[-2].expr).num_val + (yyvsp[0].expr).num_val;
        }
#line 1676 "fashion-ops-bison.tab.c"
    break;

  case 35: /* expresion: expresion MENOS termino  */
#line 447 "fashion-ops-bison.y"
        {
            if ((yyvsp[-2].expr).type != T_NUMBER || (yyvsp[0].expr).type != T_NUMBER) {
                yyerror("Error semantico: Operandos en - deben ser numericos");
            }
            (yyval.expr).type = T_NUMBER;
            (yyval.expr).num_val = (yyvsp[-2].expr).num_val - (yyvsp[0].expr).num_val;
        }
#line 1688 "fashion-ops-bison.tab.c"
    break;

  case 36: /* expresion: termino  */
#line 454 "fashion-ops-bison.y"
              { (yyval.expr) = (yyvsp[0].expr); }
#line 1694 "fashion-ops-bison.tab.c"
    break;

  case 37: /* termino: termino POR factor  */
#line 459 "fashion-ops-bison.y"
        {
            if ((yyvsp[-2].expr).type != T_NUMBER || (yyvsp[0].expr).type != T_NUMBER) {
                yyerror("Error semantico: Operandos en * deben ser numericos");
            }
            (yyval.expr).type = T_NUMBER;
            (yyval.expr).num_val = (yyvsp[-2].expr).num_val * (yyvsp[0].expr).num_val;
        }
#line 1706 "fashion-ops-bison.tab.c"
    break;

  case 38: /* termino: termino DIV factor  */
#line 467 "fashion-ops-bison.y"
        {
            if ((yyvsp[-2].expr).type != T_NUMBER || (yyvsp[0].expr).type != T_NUMBER) {
                yyerror("Error semantico: Operandos en / deben ser numericos");
            }
            (yyval.expr).type = T_NUMBER;
            (yyval.expr).num_val = (yyvsp[-2].expr).num_val / (yyvsp[0].expr).num_val;
        }
#line 1718 "fashion-ops-bison.tab.c"
    break;

  case 39: /* termino: factor  */
#line 474 "fashion-ops-bison.y"
             { (yyval.expr) = (yyvsp[0].expr); }
#line 1724 "fashion-ops-bison.tab.c"
    break;

  case 40: /* factor: NUMERO  */
#line 479 "fashion-ops-bison.y"
        {
            (yyval.expr).type = T_NUMBER;
            (yyval.expr).num_val = (yyvsp[0].valor_real);
        }
#line 1733 "fashion-ops-bison.tab.c"
    break;

  case 41: /* factor: CADENA  */
#line 484 "fashion-ops-bison.y"
        {
            (yyval.expr).type = T_STRING;
            (yyval.expr).str_val = (yyvsp[0].cadena);
        }
#line 1742 "fashion-ops-bison.tab.c"
    break;

  case 42: /* factor: IDENTIFICADOR  */
#line 489 "fashion-ops-bison.y"
        {
            Symbol *s = lookup((yyvsp[0].cadena));
            if (s == NULL) {
                yyerror("Error semantico: Identificador no declarado");
            }
            (yyval.expr).type = s->type;
            free((yyvsp[0].cadena));
        }
#line 1755 "fashion-ops-bison.tab.c"
    break;

  case 43: /* factor: acceso_arreglo  */
#line 497 "fashion-ops-bison.y"
                     { (yyval.expr) = (yyvsp[0].expr); }
#line 1761 "fashion-ops-bison.tab.c"
    break;

  case 44: /* factor: PARENTESIS_IZQ expresion PARENTESIS_DER  */
#line 498 "fashion-ops-bison.y"
                                              { (yyval.expr) = (yyvsp[-1].expr); }
#line 1767 "fashion-ops-bison.tab.c"
    break;

  case 45: /* acceso_arreglo: IDENTIFICADOR CORCHETE_IZQ expresion CORCHETE_DER  */
#line 503 "fashion-ops-bison.y"
        {
            Symbol *s = lookup((yyvsp[-3].cadena));
            if (s == NULL || s->type != T_ARRAY) {
                yyerror("Error semantico: Acceso a no-arreglo");
            }
            if ((yyvsp[-1].expr).type != T_NUMBER || (yyvsp[-1].expr).num_val < 0) {  // Verificación de índice simplificada (asume entero no negativo)
                yyerror("Error semantico: Indice de arreglo invalido");
            }
            (yyval.expr).type = s->array_elem_type;
            free((yyvsp[-3].cadena));
        }
#line 1783 "fashion-ops-bison.tab.c"
    break;

  case 46: /* lista_expresiones: %empty  */
#line 517 "fashion-ops-bison.y"
                   { (yyval.type) = T_UNKNOWN; }
#line 1789 "fashion-ops-bison.tab.c"
    break;

  case 47: /* lista_expresiones: expresion  */
#line 518 "fashion-ops-bison.y"
                { (yyval.type) = (yyvsp[0].expr).type; }
#line 1795 "fashion-ops-bison.tab.c"
    break;

  case 48: /* lista_expresiones: lista_expresiones COMA expresion  */
#line 520 "fashion-ops-bison.y"
        {
            if ((yyvsp[-2].type) != (yyvsp[0].expr).type) {
                yyerror("Error semantico: Elementos de arreglo con tipos mixtos");
            }
            (yyval.type) = (yyvsp[-2].type);
        }
#line 1806 "fashion-ops-bison.tab.c"
    break;

  case 49: /* condicion: expresion IGUAL expresion  */
#line 530 "fashion-ops-bison.y"
        {
            if ((yyvsp[-2].expr).type != (yyvsp[0].expr).type) {
                yyerror("Error semantico: Tipos incompatibles en comparacion");
            }
        }
#line 1816 "fashion-ops-bison.tab.c"
    break;

  case 50: /* condicion: expresion DIFERENTE expresion  */
#line 536 "fashion-ops-bison.y"
        {
            if ((yyvsp[-2].expr).type != (yyvsp[0].expr).type) {
                yyerror("Error semantico: Tipos incompatibles en comparacion");
            }
        }
#line 1826 "fashion-ops-bison.tab.c"
    break;

  case 51: /* condicion: expresion MAYOR expresion  */
#line 542 "fashion-ops-bison.y"
        {
            if ((yyvsp[-2].expr).type != (yyvsp[0].expr).type || (yyvsp[-2].expr).type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en >");
            }
        }
#line 1836 "fashion-ops-bison.tab.c"
    break;

  case 52: /* condicion: expresion MAYOR_IGUAL expresion  */
#line 548 "fashion-ops-bison.y"
        {
            if ((yyvsp[-2].expr).type != (yyvsp[0].expr).type || (yyvsp[-2].expr).type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en >=");
            }
        }
#line 1846 "fashion-ops-bison.tab.c"
    break;

  case 53: /* condicion: expresion MENOR expresion  */
#line 554 "fashion-ops-bison.y"
        {
            if ((yyvsp[-2].expr).type != (yyvsp[0].expr).type || (yyvsp[-2].expr).type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en <");
            }
        }
#line 1856 "fashion-ops-bison.tab.c"
    break;

  case 54: /* condicion: expresion MENOR_IGUAL expresion  */
#line 560 "fashion-ops-bison.y"
        {
            if ((yyvsp[-2].expr).type != (yyvsp[0].expr).type || (yyvsp[-2].expr).type != T_NUMBER) {
                yyerror("Error semantico: Tipos incompatibles o no numericos en <=");
            }
        }
#line 1866 "fashion-ops-bison.tab.c"
    break;

  case 55: /* condicion: NO condicion  */
#line 565 "fashion-ops-bison.y"
                   { }
#line 1872 "fashion-ops-bison.tab.c"
    break;

  case 56: /* condicion: condicion Y condicion  */
#line 566 "fashion-ops-bison.y"
                            { }
#line 1878 "fashion-ops-bison.tab.c"
    break;

  case 57: /* condicion: condicion O condicion  */
#line 567 "fashion-ops-bison.y"
                            { }
#line 1884 "fashion-ops-bison.tab.c"
    break;


#line 1888 "fashion-ops-bison.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 571 "fashion-ops-bison.y"


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
