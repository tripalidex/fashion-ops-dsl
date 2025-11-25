# FashionOpsDSL con Base de Datos SQLite
## Resumen Ejecutivo del Proyecto

---

## 📋 Descripción General

FashionOpsDSL es un lenguaje de dominio específico (DSL) diseñado para operaciones de gestión de inventario en el sector de moda. Esta implementación incluye un compilador completo con integración a base de datos SQLite que traduce comandos de alto nivel a consultas SQL.

---

## 🎯 Características Principales

### ✅ Análisis Léxico y Sintáctico
- Reconocimiento de tokens específicos del dominio
- Validación de estructura gramatical
- Manejo de errores léxicos y sintácticos

### ✅ Análisis Semántico
- Sistema de tipos estático
- Tabla de símbolos con soporte para ámbitos
- Verificación de existencia de SKUs en base de datos
- Validación de rangos y tipos de datos

### ✅ Integración con Base de Datos
- **SQLite3** embebido
- Traducción automática DSL → SQL
- Ejecución en tiempo real de consultas
- Visualización de resultados

### ✅ Operaciones Soportadas
- **REPONER**: Actualizar inventario (UPDATE)
- **APLICAR_DESCUENTO**: Registrar descuentos (INSERT)
- **VER_STOCK**: Consultar existencias (SELECT)
- **IDENTIFICAR_LENTOS**: Detectar productos de lento movimiento (SELECT)
- **REPORTAR VENTAS**: Generar reportes (SELECT con agregaciones)

---

## 🗂️ Estructura del Proyecto

```
bison-con-bd/
│
├── 📄 fashion-ops-bison.y          # Especificación Bison con lógica de BD
├── 📄 fashion-ops-dsl.l            # Especificación Flex para tokenización
│
├── 📝 test.dsl                     # Casos de prueba básicos
├── 📝 ejemplos.dsl                 # Ejemplos avanzados y casos de uso
│
├── 🗄️ consultas.sql                # Scripts SQL para inspeccionar BD
│
├── 📚 README.md                    # Documentación de usuario
├── 📚 INSTALACION.md               # Guía de instalación en Windows
├── 📚 DOCUMENTACION_TECNICA.md     # Detalles técnicos de implementación
│
├── 🔧 Makefile                     # Compilación en Linux/Mac
├── 🔧 compilar.bat                 # Compilación en Windows
│
└── 💾 fashion_ops.db               # Base de datos SQLite (generada)
```

---

## 🚀 Inicio Rápido

### Instalación (MSYS2 en Windows)
```bash
# Instalar dependencias
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-bison mingw-w64-x86_64-flex mingw-w64-x86_64-sqlite3

# Compilar
cd bison-con-bd
bison -d fashion-ops-bison.y
flex fashion-ops-dsl.l
gcc -o fashion-ops.exe fashion-ops-bison.tab.c lex.yy.c -lsqlite3

# Ejecutar
./fashion-ops.exe test.dsl
```

### Uso Básico
```dsl
// Reponer inventario
REPONER camisa_M 50;

// Aplicar descuento
APLICAR_DESCUENTO camisa_M 25%;

// Consultar stock
VER_STOCK camisa_M ES cantidad;

// Identificar productos lentos
IDENTIFICAR_LENTOS DIAS MAYOR 10;
```

---

## 📊 Esquema de Base de Datos

### Tabla: productos
| Campo | Tipo | Descripción |
|-------|------|-------------|
| id | INTEGER | Clave primaria |
| sku | TEXT | Código único del producto |
| nombre | TEXT | Nombre descriptivo |
| stock | INTEGER | Cantidad en inventario |
| precio | REAL | Precio unitario |
| dias_sin_venta | INTEGER | Días sin movimiento |

### Tabla: descuentos
| Campo | Tipo | Descripción |
|-------|------|-------------|
| id | INTEGER | Clave primaria |
| sku | TEXT | Producto con descuento |
| porcentaje | REAL | % de descuento |
| fecha_aplicacion | DATETIME | Timestamp |

### Tabla: ventas
| Campo | Tipo | Descripción |
|-------|------|-------------|
| id | INTEGER | Clave primaria |
| sku | TEXT | Producto vendido |
| cantidad | INTEGER | Unidades vendidas |
| total | REAL | Monto total |
| fecha | DATE | Fecha de venta |

---

## 🔄 Mapeo DSL → SQL

### Ejemplo 1: REPONER
```dsl
REPONER camisa_M 50;
```
**SQL Generado:**
```sql
UPDATE productos SET stock = stock + 50 WHERE sku = 'camisa_M';
```

### Ejemplo 2: APLICAR_DESCUENTO
```dsl
APLICAR_DESCUENTO camisa_M 25%;
```
**SQL Generado:**
```sql
INSERT INTO descuentos (sku, porcentaje) VALUES ('camisa_M', 25.0);
```

### Ejemplo 3: VER_STOCK
```dsl
VER_STOCK camisa_M ES cantidad;
```
**SQL Generado:**
```sql
SELECT stock FROM productos WHERE sku = 'camisa_M';
```

### Ejemplo 4: IDENTIFICAR_LENTOS
```dsl
IDENTIFICAR_LENTOS DIAS MAYOR 10;
```
**SQL Generado:**
```sql
SELECT sku, nombre, stock, dias_sin_venta 
FROM productos 
WHERE dias_sin_venta > 10;
```

### Ejemplo 5: REPORTAR VENTAS
```dsl
REPORTAR VENTAS DIARIO;
```
**SQL Generado:**
```sql
SELECT fecha, SUM(total) as total_ventas, SUM(cantidad) as items_vendidos 
FROM ventas 
WHERE fecha = DATE('now') 
GROUP BY fecha;
```

---

## 🛠️ Tecnologías Utilizadas

| Componente | Tecnología | Versión |
|------------|-----------|---------|
| Analizador Léxico | GNU Flex | 2.6.4+ |
| Analizador Sintáctico | GNU Bison | 3.8+ |
| Compilador | GCC | 10.0+ |
| Base de Datos | SQLite3 | 3.35+ |
| Sistema Build | Make / Batch | - |

---

## 📈 Ventajas del Enfoque

### 1. **Abstracción de SQL**
Los usuarios pueden interactuar con la base de datos sin conocer SQL:
```dsl
REPONER camisa_M 50;  // Simple y declarativo
```
vs.
```sql
UPDATE productos SET stock = stock + 50 WHERE sku = 'camisa_M';
```

### 2. **Validación en Tiempo de Compilación**
- Verificación de tipos
- Existencia de SKUs
- Rangos válidos de valores

### 3. **Seguridad**
- No inyección SQL (valores escapados)
- Validación de entrada
- Transacciones implícitas

### 4. **Mantenibilidad**
- Código más legible
- Menos propenso a errores
- Fácil de extender

---

## 🧪 Casos de Prueba

### Test 1: Operaciones Básicas
```dsl
REPONER camisa_M 50;
APLICAR_DESCUENTO camisa_M 25%;
VER_STOCK camisa_M ES cantidad;
```

### Test 2: Identificación de Lentos
```dsl
IDENTIFICAR_LENTOS DIAS MAYOR 10;
IDENTIFICAR_LENTOS DIAS MAYOR 20 VENTAS MENOR 5;
```

### Test 3: Reportes
```dsl
REPORTAR VENTAS DIARIO;
REPORTAR VENTAS MENSUAL;
```

### Test 4: Control de Flujo
```dsl
SI cantidad MENOR 20 ENTONCES
    REPONER camisa_M 50;
FIN
```

---

## 📝 Ejemplo de Salida

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

=== Análisis completado exitosamente ===
```

---

## 🎓 Conceptos de Compiladores Implementados

- ✅ Análisis Léxico (Tokenización)
- ✅ Análisis Sintáctico (Parsing LL/LR)
- ✅ Análisis Semántico (Verificación de tipos)
- ✅ Tabla de Símbolos (Manejo de ámbitos)
- ✅ Generación de Código (SQL)
- ✅ Ejecución en Tiempo Real

---

## 🚧 Limitaciones Actuales

1. No soporta transacciones explícitas
2. Arreglos solo homogéneos
3. No hay optimización de consultas
4. Sin soporte para JOIN explícitos en DSL
5. Verificación de SKU en cada operación (sin caché)

---

## 🔮 Mejoras Futuras

### Corto Plazo
- [ ] Caché de SKUs
- [ ] Transacciones automáticas
- [ ] Mejor manejo de errores
- [ ] Más operadores lógicos

### Mediano Plazo
- [ ] Generación de reportes HTML/JSON
- [ ] Soporte para múltiples bases de datos
- [ ] Interfaz gráfica
- [ ] API REST

### Largo Plazo
- [ ] Compilación JIT
- [ ] Optimizador de consultas
- [ ] Soporte distribuido
- [ ] Machine Learning para predicciones

---

## 👥 Casos de Uso

### 1. **Retail Manager**
Gestionar inventario sin conocer SQL:
```dsl
IDENTIFICAR_LENTOS DIAS MAYOR 30;
APLICAR_DESCUENTO producto_lento 40%;
```

### 2. **Analista de Datos**
Generar reportes rápidos:
```dsl
REPORTAR VENTAS MENSUAL;
```

### 3. **Automatización**
Scripts para tareas recurrentes:
```dsl
// Reponer automáticamente
VER_STOCK camisa_M ES stock;
SI stock MENOR 20 ENTONCES
    REPONER camisa_M 100;
FIN
```

---

## 📚 Documentación Completa

- **README.md**: Guía de usuario y ejemplos
- **INSTALACION.md**: Proceso de instalación detallado
- **DOCUMENTACION_TECNICA.md**: Arquitectura y detalles internos
- **ejemplos.dsl**: Casos de uso completos
- **consultas.sql**: Scripts SQL para inspeccionar BD

---

## 🤝 Contribuciones

Este proyecto es parte del curso de Compiladores y demuestra la integración de:
- Teoría de lenguajes formales
- Análisis sintáctico y semántico
- Generación de código
- Integración con sistemas externos (BD)

---

## 📄 Licencia

Proyecto académico - Universidad [Nombre]
Curso: Compiladores
Año: 2025

---

## ✉️ Contacto

Para preguntas o sugerencias sobre el proyecto:
- Repositorio: https://github.com/tripalidex/fashion-ops-dsl
- Documentación: Ver archivos .md en el repositorio

---

**¡Gracias por usar FashionOpsDSL!** 🎉
