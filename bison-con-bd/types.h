#ifndef TYPES_H
#define TYPES_H

// Enumeración para tipos
typedef enum { T_UNKNOWN, T_NUMBER, T_STRING, T_ARRAY } Type;

// Estructura para expresiones
typedef struct {
    Type type;
    double num_val;
    char *str_val;
} Expr;

#endif
