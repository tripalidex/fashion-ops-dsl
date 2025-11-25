# FashionOpsDSL con Base de Datos SQLite

Este proyecto implementa un analizador sintáctico y semántico para el lenguaje FashionOpsDSL con integración a base de datos SQLite.

## Características

- **Análisis léxico**: Reconocimiento de tokens del DSL
- **Análisis sintáctico**: Validación de la estructura del código
- **Análisis semántico**: Verificación de tipos y símbolos
- **Integración con SQLite**: Traduce operaciones del DSL a SQL y las ejecuta

## Mapeo DSL a SQL

El analizador traduce las operaciones del DSL a consultas SQL:

### REPONER
```dsl
REPONER camisa_M 50;
```
**SQL generado:**
```sql
UPDATE productos SET stock = stock + 50 WHERE sku = 'camisa_M';
```

### APLICAR_DESCUENTO
```dsl
APLICAR_DESCUENTO camisa_M 25%;
```
**SQL generado:**
```sql
INSERT INTO descuentos (sku, porcentaje) VALUES ('camisa_M', 25.0);
```

### VER_STOCK
```dsl
VER_STOCK camisa_M ES cantidad;
```
**SQL generado:**
```sql
SELECT stock FROM productos WHERE sku = 'camisa_M';
```

### IDENTIFICAR_LENTOS
```dsl
IDENTIFICAR_LENTOS DIAS MAYOR 10;
```
**SQL generado:**
```sql
SELECT sku, nombre, stock, dias_sin_venta FROM productos WHERE dias_sin_venta > 10;
```

### REPORTAR VENTAS
```dsl
REPORTAR VENTAS DIARIO;
```
**SQL generado:**
```sql
SELECT fecha, SUM(total) as total_ventas, SUM(cantidad) as items_vendidos 
FROM ventas 
WHERE fecha = DATE('now') 
GROUP BY fecha;
```

## Estructura de la Base de Datos

### Tabla: productos
- `id` (INTEGER PRIMARY KEY)
- `sku` (TEXT UNIQUE)
- `nombre` (TEXT)
- `stock` (INTEGER)
- `precio` (REAL)
- `dias_sin_venta` (INTEGER)

### Tabla: descuentos
- `id` (INTEGER PRIMARY KEY)
- `sku` (TEXT)
- `porcentaje` (REAL)
- `fecha_aplicacion` (DATETIME)

### Tabla: ventas
- `id` (INTEGER PRIMARY KEY)
- `sku` (TEXT)
- `cantidad` (INTEGER)
- `total` (REAL)
- `fecha` (DATE)

## Compilación

### Requisitos
- GCC o MinGW
- Bison
- Flex
- SQLite3

### Windows (con MinGW/MSYS2)
```powershell
# Instalar dependencias con MSYS2
pacman -S mingw-w64-x86_64-bison mingw-w64-x86_64-flex mingw-w64-x86_64-sqlite3

# Compilar
bison -d fashion-ops-bison.y
flex fashion-ops-dsl.l
gcc -o fashion-ops fashion-ops-bison.tab.c lex.yy.c -lsqlite3
```

### Linux
```bash
# Instalar dependencias
sudo apt-get install bison flex libsqlite3-dev

# Usar Makefile
make
```

## Uso

```bash
# Ejecutar con archivo de entrada
./fashion-ops test.dsl

# Ejecutar modo interactivo
./fashion-ops
```

## Ejemplo de Ejecución

```
Base de datos SQLite conectada: fashion_ops.db

>> SQL generado: CREATE TABLE IF NOT EXISTS productos...
>> Operación ejecutada exitosamente

=== Iniciando análisis sintáctico y semántico de FashionOpsDSL ===

>> SQL generado: UPDATE productos SET stock = stock + 50 WHERE sku = 'camisa_M';
>> Operación ejecutada exitosamente
Repuesto producto 'camisa_M' con cantidad 50

>> SQL generado: INSERT INTO descuentos (sku, porcentaje) VALUES ('camisa_M', 25.00);
>> Operación ejecutada exitosamente
Aplicado 25% descuento a 'camisa_M'

>> SQL generado: SELECT stock FROM productos WHERE sku = 'camisa_M';
stock = 150

>> Operación ejecutada exitosamente
Variable 'cantidad' asignada con stock de 'camisa_M'

=== Análisis completado exitosamente ===
```

## Archivos del Proyecto

- `fashion-ops-bison.y` - Especificación del parser Bison con integración SQLite
- `fashion-ops-dsl.l` - Especificación del lexer Flex
- `test.dsl` - Archivo de prueba con comandos del DSL
- `Makefile` - Script de compilación
- `fashion_ops.db` - Base de datos SQLite (generada automáticamente)

## Notas

- La base de datos se crea automáticamente al iniciar el programa
- Se insertan datos de ejemplo (camisa_M, pantalon_azul, zapato_42)
- El programa muestra tanto el SQL generado como los resultados de las consultas
- La validación semántica verifica que los SKUs existan en la base de datos antes de ejecutar operaciones
