# Documentación Técnica - FashionOpsDSL con SQLite

## Arquitectura del Sistema

```
┌─────────────────┐
│   Código DSL    │
│   (test.dsl)    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Analizador     │
│   Léxico (Flex) │
└────────┬────────┘
         │ (tokens)
         ▼
┌─────────────────┐
│  Analizador     │
│ Sintáctico/Sem. │
│    (Bison)      │
└────────┬────────┘
         │ (SQL)
         ▼
┌─────────────────┐
│  Base de Datos  │
│    SQLite3      │
└─────────────────┘
```

## Componentes Principales

### 1. Analizador Léxico (fashion-ops-dsl.l)

**Responsabilidades:**
- Tokenización del código fuente
- Reconocimiento de palabras clave, identificadores, números y operadores
- Manejo de comentarios

**Tokens principales:**
```
Palabras clave: REPORTAR, VENTAS, REPONER, VER_STOCK, etc.
Operadores: +, -, *, /, ==, !=, >, <, etc.
Literales: NUMERO, CADENA, IDENTIFICADOR
Delimitadores: ; , ( ) [ ]
```

### 2. Analizador Sintáctico y Semántico (fashion-ops-bison.y)

**Gramática BNF simplificada:**
```bnf
programa ::= sentencia*
sentencia ::= declaracion | asignacion | reporte | ver_stock | 
              reponer | identificar_lentos | aplicar_descuento
declaracion ::= VARIABLE id = expr | CONSTANTE id = expr | ARREGLO id = [lista]
reporte ::= REPORTAR VENTAS (DIARIO | MENSUAL) [FILTRAR condicion]
ver_stock ::= VER_STOCK id ES id
reponer ::= REPONER id expr
identificar_lentos ::= IDENTIFICAR_LENTOS DIAS MAYOR num [VENTAS MENOR num]
aplicar_descuento ::= APLICAR_DESCUENTO id num % [SI condicion]
```

### 3. Sistema de Tipos

```c
typedef enum { 
    T_UNKNOWN,  // Tipo desconocido
    T_NUMBER,   // Números (int/float)
    T_STRING,   // Cadenas de texto
    T_ARRAY     // Arreglos homogéneos
} Type;
```

### 4. Tabla de Símbolos

**Estructura:**
```c
typedef struct Symbol {
    char *name;              // Nombre del símbolo
    Type type;               // Tipo del símbolo
    int is_const;            // ¿Es constante?
    int scope;               // Ámbito actual
    Type array_elem_type;    // Tipo de elementos (si es array)
    struct Symbol *next;     // Siguiente en la lista
} Symbol;
```

**Operaciones:**
- `lookup(name)`: Busca un símbolo en la tabla
- `insert(name, type, is_const, array_type)`: Inserta un nuevo símbolo
- `enter_scope()`: Incrementa el nivel de ámbito
- `exit_scope()`: Sale del ámbito y elimina símbolos locales

### 5. Integración con SQLite

**Funciones principales:**

```c
// Inicializar conexión y crear tablas
void init_database()

// Ejecutar SQL y mostrar resultados
void ejecutar_sql(const char *sql)

// Callback para resultados de SELECT
int callback(void *NotUsed, int argc, char **argv, char **azColName)

// Verificar existencia de SKU en BD
int is_sku(char *id)
```

## Mapeo DSL → SQL

### Tabla de Conversiones

| Operación DSL | SQL Generado | Tipo |
|--------------|--------------|------|
| `REPONER sku qty` | `UPDATE productos SET stock = stock + qty WHERE sku = 'sku'` | UPDATE |
| `APLICAR_DESCUENTO sku pct%` | `INSERT INTO descuentos (sku, porcentaje) VALUES ('sku', pct)` | INSERT |
| `VER_STOCK sku ES var` | `SELECT stock FROM productos WHERE sku = 'sku'` | SELECT |
| `IDENTIFICAR_LENTOS DIAS > n` | `SELECT ... FROM productos WHERE dias_sin_venta > n` | SELECT |
| `REPORTAR VENTAS DIARIO` | `SELECT ... FROM ventas WHERE fecha = DATE('now')` | SELECT |

## Análisis Semántico

### Verificaciones implementadas:

1. **Declaración de variables:**
   - No redeclaración en el mismo ámbito
   - Tipo coherente con la expresión asignada

2. **Uso de variables:**
   - Variable declarada antes de usar
   - No asignación a constantes

3. **Operaciones aritméticas:**
   - Operandos deben ser numéricos
   - Tipo resultado es numérico

4. **Comparaciones:**
   - Ambos operandos del mismo tipo
   - Comparaciones de orden (>, <, etc.) solo para números

5. **Arreglos:**
   - Elementos homogéneos
   - Índices numéricos no negativos

6. **Operaciones de negocio:**
   - SKU existe en base de datos
   - Cantidades positivas en REPONER
   - Porcentajes entre 0-100 en APLICAR_DESCUENTO

## Flujo de Ejecución

```
1. main()
   ├─ init_database()
   │  ├─ sqlite3_open()
   │  ├─ CREATE TABLE productos
   │  ├─ CREATE TABLE descuentos
   │  ├─ CREATE TABLE ventas
   │  └─ INSERT datos de ejemplo
   │
   ├─ yyparse()
   │  ├─ yylex() (tokenizar)
   │  ├─ Aplicar reglas gramaticales
   │  ├─ Verificar semántica
   │  └─ ejecutar_sql()
   │     ├─ printf(SQL)
   │     ├─ sqlite3_exec()
   │     └─ callback() (mostrar resultados)
   │
   └─ sqlite3_close()
```

## Manejo de Errores

### Errores Léxicos
- Caracteres no reconocidos
- Formato inválido de literales

### Errores Sintácticos
- Estructura gramatical incorrecta
- Tokens inesperados
- Falta de delimitadores

### Errores Semánticos
- Variable no declarada
- Tipos incompatibles
- SKU inexistente
- Valores fuera de rango
- Asignación a constante

## Optimizaciones Posibles

### 1. Tabla de Símbolos
- Usar hash table en lugar de lista enlazada
- Mejorar búsqueda de O(n) a O(1)

### 2. Caché de Consultas
```c
typedef struct {
    char *sku;
    int stock;
    time_t timestamp;
} CacheEntry;
```

### 3. Transacciones SQL
```sql
BEGIN TRANSACTION;
  UPDATE productos ...;
  INSERT INTO descuentos ...;
COMMIT;
```

### 4. Prepared Statements
```c
sqlite3_stmt *stmt;
sqlite3_prepare_v2(db, "UPDATE productos SET stock = ? WHERE sku = ?", -1, &stmt, NULL);
sqlite3_bind_int(stmt, 1, cantidad);
sqlite3_bind_text(stmt, 2, sku, -1, SQLITE_TRANSIENT);
```

## Extensiones Futuras

### 1. Generación de Código Intermedio
```c
typedef enum {
    OP_LOAD, OP_STORE, OP_ADD, OP_SUB,
    OP_SQL_UPDATE, OP_SQL_SELECT, OP_SQL_INSERT
} OpCode;

typedef struct {
    OpCode op;
    void *arg1;
    void *arg2;
} Instruction;
```

### 2. Optimizador de Consultas
- Combinar múltiples operaciones en una transacción
- Eliminar consultas redundantes
- Reordenar para eficiencia

### 3. Sistema de Tipos Avanzado
```c
typedef struct {
    Type base_type;
    int is_nullable;
    int precision;  // Para decimales
    int size;       // Para arreglos
} TypeInfo;
```

### 4. Generación de Reportes
- HTML
- JSON
- CSV
- PDF

## Testing

### Casos de Prueba

```bash
# Test 1: Operaciones básicas
./fashion-ops test.dsl

# Test 2: Casos extremos
./fashion-ops test_edge_cases.dsl

# Test 3: Errores semánticos
./fashion-ops test_errors.dsl

# Test 4: Performance
time ./fashion-ops test_large.dsl
```

### Verificación de Base de Datos
```bash
sqlite3 fashion_ops.db "SELECT * FROM productos;"
sqlite3 fashion_ops.db "SELECT * FROM descuentos;"
```

## Métricas del Compilador

- **Tokens reconocidos**: ~40
- **Reglas gramaticales**: ~50
- **Tipos soportados**: 4 (UNKNOWN, NUMBER, STRING, ARRAY)
- **Operaciones SQL**: 3 (SELECT, INSERT, UPDATE)
- **Tablas BD**: 3 (productos, descuentos, ventas)

## Referencias

- **Bison Manual**: https://www.gnu.org/software/bison/manual/
- **Flex Manual**: https://westes.github.io/flex/manual/
- **SQLite API**: https://www.sqlite.org/c3ref/intro.html
- **Compilers (Dragon Book)**: Aho, Lam, Sethi, Ullman
- **Modern Compiler Implementation**: Andrew Appel
