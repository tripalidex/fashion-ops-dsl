# 📚 Índice de Documentación - FashionOpsDSL

## Bienvenido al proyecto FashionOpsDSL

Este es un compilador completo con integración a base de datos SQLite para un lenguaje de dominio específico orientado a operaciones de moda e inventario.

---

## 🗂️ Estructura de Archivos

### 📘 Documentación

| Archivo | Descripción | Nivel |
|---------|-------------|-------|
| [RESUMEN_PROYECTO.md](RESUMEN_PROYECTO.md) | Vista general del proyecto | ⭐ Empezar aquí |
| [README.md](README.md) | Guía de usuario y ejemplos | ⭐⭐ Básico |
| [INSTALACION.md](INSTALACION.md) | Instalación en Windows | ⭐⭐ Básico |
| [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md) | Detalles técnicos | ⭐⭐⭐ Avanzado |

### 💻 Código Fuente

| Archivo | Descripción | Lenguaje |
|---------|-------------|----------|
| fashion-ops-bison.y | Analizador sintáctico/semántico + BD | Bison/C |
| fashion-ops-dsl.l | Analizador léxico | Flex |

### 📝 Ejemplos y Pruebas

| Archivo | Descripción | Uso |
|---------|-------------|-----|
| test.dsl | Casos de prueba básicos | Testing |
| ejemplos.dsl | Ejemplos completos de uso | Aprendizaje |
| consultas.sql | Scripts para inspeccionar BD | Debug |

### 🔧 Herramientas de Build

| Archivo | Descripción | Plataforma |
|---------|-------------|------------|
| Makefile | Compilación con make | Linux/Mac |
| compilar.bat | Script de compilación | Windows |

---

## 🚀 Guía de Inicio Rápido

### Para Usuarios Nuevos

1. **Leer primero**: [RESUMEN_PROYECTO.md](RESUMEN_PROYECTO.md)
   - Entender qué hace el proyecto
   - Ver ejemplos de uso

2. **Instalar**: [INSTALACION.md](INSTALACION.md)
   - Seguir guía de instalación para tu SO
   - Instalar dependencias (GCC, Bison, Flex, SQLite)

3. **Compilar**:
   ```bash
   # Windows
   compilar.bat
   
   # Linux/Mac
   make
   ```

4. **Ejecutar**: 
   ```bash
   ./fashion-ops test.dsl
   ```

5. **Aprender**: [ejemplos.dsl](ejemplos.dsl)
   - Ver casos de uso completos
   - Experimentar con el código

### Para Desarrolladores

1. **Arquitectura**: [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md)
   - Entender la implementación interna
   - Ver diagramas y flujos

2. **Código Fuente**:
   - `fashion-ops-bison.y`: Lógica principal
   - `fashion-ops-dsl.l`: Tokenización

3. **Extender**:
   - Agregar nuevas operaciones
   - Modificar gramática
   - Añadir validaciones

---

## 📖 Contenido por Tema

### 🎯 Características del Lenguaje

**Archivo**: [README.md](README.md) - Sección "Mapeo DSL a SQL"

**Operaciones soportadas:**
- REPONER (UPDATE)
- APLICAR_DESCUENTO (INSERT)
- VER_STOCK (SELECT)
- IDENTIFICAR_LENTOS (SELECT con filtros)
- REPORTAR VENTAS (SELECT con agregaciones)

**Tipos de datos:**
- Números (enteros y decimales)
- Cadenas de texto
- Arreglos homogéneos

**Estructuras de control:**
- SI-ENTONCES-SINO-FIN
- PARA-EN-HAZ-FIN

---

### 🗄️ Base de Datos

**Archivo**: [README.md](README.md) - Sección "Estructura de la Base de Datos"

**Tablas:**
- `productos`: Inventario principal
- `descuentos`: Historial de descuentos
- `ventas`: Registro de ventas

**Archivo de consultas**: [consultas.sql](consultas.sql)

---

### 🔧 Instalación y Configuración

**Archivo principal**: [INSTALACION.md](INSTALACION.md)

**Opciones de instalación:**
1. MSYS2 (Recomendado para Windows)
2. WinFlexBison + Visual Studio
3. Cygwin

**Solución de problemas**: Ver sección en INSTALACION.md

---

### 🏗️ Arquitectura Técnica

**Archivo principal**: [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md)

**Temas cubiertos:**
- Arquitectura del sistema
- Componentes principales
- Sistema de tipos
- Tabla de símbolos
- Flujo de ejecución
- Manejo de errores
- Optimizaciones

---

### 💡 Ejemplos de Uso

**Archivo**: [ejemplos.dsl](ejemplos.dsl)

**Categorías de ejemplos:**
1. Gestión de inventario
2. Aplicación de descuentos
3. Identificación de productos lentos
4. Reportes de ventas
5. Variables y constantes
6. Estructuras de control
7. Iteración con arreglos
8. Operaciones combinadas
9. Casos de uso reales
10. Análisis de inventario

---

## 🎓 Conceptos de Compiladores

### Análisis Léxico
**Ver**: fashion-ops-dsl.l + [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md)
- Tokenización
- Expresiones regulares
- Manejo de comentarios

### Análisis Sintáctico
**Ver**: fashion-ops-bison.y + [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md)
- Gramática BNF
- Parsing LR
- Manejo de precedencia

### Análisis Semántico
**Ver**: fashion-ops-bison.y (sección de código C)
- Sistema de tipos
- Tabla de símbolos
- Verificación de ámbitos
- Validación de restricciones

### Generación de Código
**Ver**: fashion-ops-bison.y (acciones semánticas)
- Traducción DSL → SQL
- Ejecución de código generado

---

## 🧪 Testing y Validación

### Casos de Prueba Básicos
**Archivo**: [test.dsl](test.dsl)

### Casos de Prueba Avanzados
**Archivo**: [ejemplos.dsl](ejemplos.dsl)

### Verificación de Base de Datos
**Archivo**: [consultas.sql](consultas.sql)
```bash
sqlite3 fashion_ops.db < consultas.sql
```

---

## 🔍 Búsqueda Rápida

### ¿Cómo hago...?

| Pregunta | Ir a |
|----------|------|
| ¿Instalar el proyecto? | [INSTALACION.md](INSTALACION.md) |
| ¿Compilar el código? | [README.md](README.md) - Sección "Compilación" |
| ¿Ejecutar un programa? | [README.md](README.md) - Sección "Uso" |
| ¿Ver ejemplos de código? | [ejemplos.dsl](ejemplos.dsl) |
| ¿Entender la arquitectura? | [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md) |
| ¿Ver las tablas de BD? | [README.md](README.md) - Sección "Estructura de BD" |
| ¿Agregar nuevas operaciones? | [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md) - Sección "Extensiones" |
| ¿Solucionar errores de compilación? | [INSTALACION.md](INSTALACION.md) - Sección "Solución de Problemas" |

---

## 📊 Diagramas y Visualizaciones

### Flujo de Datos
```
DSL Code → Lexer → Parser → SQL Generator → SQLite → Results
```

**Detalles**: [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md) - "Arquitectura del Sistema"

### Estructura de Tablas
**Ver**: [README.md](README.md) - "Estructura de la Base de Datos"

---

## 🎯 Casos de Uso

### Por Rol

| Rol | Documentación Recomendada |
|-----|---------------------------|
| Usuario Final | [README.md](README.md), [ejemplos.dsl](ejemplos.dsl) |
| Desarrollador | [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md) |
| Instructor | [RESUMEN_PROYECTO.md](RESUMEN_PROYECTO.md) |
| Estudiante | Todos los archivos en orden |

### Por Objetivo

| Objetivo | Archivos Clave |
|----------|----------------|
| Aprender compiladores | fashion-ops-bison.y, fashion-ops-dsl.l, [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md) |
| Usar el DSL | [README.md](README.md), [ejemplos.dsl](ejemplos.dsl) |
| Extender el proyecto | [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md), código fuente |
| Integrar con otros sistemas | [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md) - "Integración con SQLite" |

---

## 🆘 Soporte

### Documentación de Dependencias

- **Bison**: https://www.gnu.org/software/bison/manual/
- **Flex**: https://westes.github.io/flex/manual/
- **SQLite**: https://www.sqlite.org/docs.html
- **GCC**: https://gcc.gnu.org/onlinedocs/

### Recursos Adicionales

- **Teoría de Compiladores**: Dragon Book (Aho et al.)
- **Parsing**: "Parsing Techniques" (Grune & Jacobs)
- **SQLite**: "The Definitive Guide to SQLite" (Allen & Owens)

---

## 📝 Checklist de Aprendizaje

### Nivel 1: Básico
- [ ] Instalar dependencias
- [ ] Compilar el proyecto
- [ ] Ejecutar test.dsl
- [ ] Entender sintaxis básica
- [ ] Ver resultados en BD

### Nivel 2: Intermedio
- [ ] Escribir programas propios
- [ ] Entender análisis léxico
- [ ] Entender análisis sintáctico
- [ ] Modificar ejemplos
- [ ] Agregar datos a BD

### Nivel 3: Avanzado
- [ ] Entender análisis semántico
- [ ] Modificar gramática
- [ ] Agregar nuevas operaciones
- [ ] Optimizar consultas SQL
- [ ] Extender el sistema de tipos

---

## 🏆 Logros del Proyecto

✅ Compilador completo funcional  
✅ Integración con base de datos  
✅ Análisis semántico robusto  
✅ Documentación exhaustiva  
✅ Casos de prueba completos  
✅ Scripts de compilación multiplataforma  
✅ Ejemplos de uso extensivos  

---

## 📞 Información del Proyecto

- **Repositorio**: https://github.com/tripalidex/fashion-ops-dsl
- **Carpeta del proyecto**: `bison-con-bd/`
- **Base de datos**: `fashion_ops.db` (generada automáticamente)
- **Ejecutable**: `fashion-ops` / `fashion-ops.exe`

---

**¡Navega por la documentación y disfruta aprendiendo sobre compiladores!** 🚀📚

**Última actualización**: Noviembre 2025
