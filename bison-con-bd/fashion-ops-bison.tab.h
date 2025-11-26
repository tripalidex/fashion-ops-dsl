/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_FASHION_OPS_BISON_TAB_H_INCLUDED
# define YY_YY_FASHION_OPS_BISON_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NUMERO = 258,                  /* NUMERO  */
    CADENA = 259,                  /* CADENA  */
    IDENTIFICADOR = 260,           /* IDENTIFICADOR  */
    REPORTAR = 261,                /* REPORTAR  */
    VENTAS = 262,                  /* VENTAS  */
    DIARIO = 263,                  /* DIARIO  */
    MENSUAL = 264,                 /* MENSUAL  */
    FILTRAR = 265,                 /* FILTRAR  */
    VER_STOCK = 266,               /* VER_STOCK  */
    REPONER = 267,                 /* REPONER  */
    IDENTIFICAR_LENTOS = 268,      /* IDENTIFICAR_LENTOS  */
    APLICAR_DESCUENTO = 269,       /* APLICAR_DESCUENTO  */
    SI = 270,                      /* SI  */
    ENTONCES = 271,                /* ENTONCES  */
    SINO = 272,                    /* SINO  */
    FIN = 273,                     /* FIN  */
    PARA = 274,                    /* PARA  */
    EN = 275,                      /* EN  */
    HAZ = 276,                     /* HAZ  */
    VARIABLE = 277,                /* VARIABLE  */
    CONSTANTE = 278,               /* CONSTANTE  */
    ARREGLO = 279,                 /* ARREGLO  */
    ES = 280,                      /* ES  */
    DIAS = 281,                    /* DIAS  */
    PORCENTAJE = 282,              /* PORCENTAJE  */
    Y = 283,                       /* Y  */
    O = 284,                       /* O  */
    NO = 285,                      /* NO  */
    IGUAL = 286,                   /* IGUAL  */
    DIFERENTE = 287,               /* DIFERENTE  */
    MAYOR_IGUAL = 288,             /* MAYOR_IGUAL  */
    MENOR_IGUAL = 289,             /* MENOR_IGUAL  */
    MAYOR = 290,                   /* MAYOR  */
    MENOR = 291,                   /* MENOR  */
    MAS = 292,                     /* MAS  */
    MENOS = 293,                   /* MENOS  */
    POR = 294,                     /* POR  */
    DIV = 295,                     /* DIV  */
    ASIGNAR = 296,                 /* ASIGNAR  */
    PUNTO_COMA = 297,              /* PUNTO_COMA  */
    COMA = 298,                    /* COMA  */
    PARENTESIS_IZQ = 299,          /* PARENTESIS_IZQ  */
    PARENTESIS_DER = 300,          /* PARENTESIS_DER  */
    CORCHETE_IZQ = 301,            /* CORCHETE_IZQ  */
    CORCHETE_DER = 302             /* CORCHETE_DER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 175 "fashion-ops-bison.y"

    double valor_real;
    char *cadena;
    Expr expr;
    Type type;  // Para tipos de lista en arreglos

#line 118 "fashion-ops-bison.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_FASHION_OPS_BISON_TAB_H_INCLUDED  */
