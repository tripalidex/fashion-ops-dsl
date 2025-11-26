/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NUMERO = 258,
     CADENA = 259,
     IDENTIFICADOR = 260,
     REPORTAR = 261,
     VENTAS = 262,
     DIARIO = 263,
     MENSUAL = 264,
     FILTRAR = 265,
     VER_STOCK = 266,
     REPONER = 267,
     IDENTIFICAR_LENTOS = 268,
     APLICAR_DESCUENTO = 269,
     SI = 270,
     ENTONCES = 271,
     SINO = 272,
     FIN = 273,
     PARA = 274,
     EN = 275,
     HAZ = 276,
     VARIABLE = 277,
     CONSTANTE = 278,
     ARREGLO = 279,
     ES = 280,
     DIAS = 281,
     PORCENTAJE = 282,
     Y = 283,
     O = 284,
     NO = 285,
     IGUAL = 286,
     DIFERENTE = 287,
     MAYOR_IGUAL = 288,
     MENOR_IGUAL = 289,
     MAYOR = 290,
     MENOR = 291,
     MAS = 292,
     MENOS = 293,
     POR = 294,
     DIV = 295,
     ASIGNAR = 296,
     PUNTO_COMA = 297,
     COMA = 298,
     PARENTESIS_IZQ = 299,
     PARENTESIS_DER = 300,
     CORCHETE_IZQ = 301,
     CORCHETE_DER = 302
   };
#endif
/* Tokens.  */
#define NUMERO 258
#define CADENA 259
#define IDENTIFICADOR 260
#define REPORTAR 261
#define VENTAS 262
#define DIARIO 263
#define MENSUAL 264
#define FILTRAR 265
#define VER_STOCK 266
#define REPONER 267
#define IDENTIFICAR_LENTOS 268
#define APLICAR_DESCUENTO 269
#define SI 270
#define ENTONCES 271
#define SINO 272
#define FIN 273
#define PARA 274
#define EN 275
#define HAZ 276
#define VARIABLE 277
#define CONSTANTE 278
#define ARREGLO 279
#define ES 280
#define DIAS 281
#define PORCENTAJE 282
#define Y 283
#define O 284
#define NO 285
#define IGUAL 286
#define DIFERENTE 287
#define MAYOR_IGUAL 288
#define MENOR_IGUAL 289
#define MAYOR 290
#define MENOR 291
#define MAS 292
#define MENOS 293
#define POR 294
#define DIV 295
#define ASIGNAR 296
#define PUNTO_COMA 297
#define COMA 298
#define PARENTESIS_IZQ 299
#define PARENTESIS_DER 300
#define CORCHETE_IZQ 301
#define CORCHETE_DER 302




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 175 "fashion-ops-bison.y"
{
    double valor_real;
    char *cadena;
    Expr expr;
    Type type;  // Para tipos de lista en arreglos
}
/* Line 1529 of yacc.c.  */
#line 150 "fashion-ops-bison.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

