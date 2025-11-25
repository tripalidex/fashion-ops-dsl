# Guía de Instalación - FashionOpsDSL con SQLite en Windows

## Opción 1: Usar MSYS2 (Recomendado)

### Paso 1: Instalar MSYS2
1. Descargar MSYS2 desde: https://www.msys2.org/
2. Ejecutar el instalador y seguir las instrucciones
3. Abrir "MSYS2 MinGW 64-bit" desde el menú inicio

### Paso 2: Instalar Herramientas
```bash
# Actualizar el sistema
pacman -Syu

# Instalar compilador y herramientas
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-bison
pacman -S mingw-w64-x86_64-flex
pacman -S mingw-w64-x86_64-sqlite3

# Instalar make (opcional)
pacman -S make
```

### Paso 3: Compilar el Proyecto
```bash
cd /c/Users/sebar/Documents/Repositories/fashion-ops-dsl/bison-con-bd

# Opción A: Usar el script de compilación
./compilar.bat

# Opción B: Comandos manuales
bison -d fashion-ops-bison.y
flex fashion-ops-dsl.l
gcc -o fashion-ops.exe fashion-ops-bison.tab.c lex.yy.c -lsqlite3
```

### Paso 4: Ejecutar
```bash
./fashion-ops.exe test.dsl
```

---

## Opción 2: Usar WinFlexBison con Visual Studio

### Paso 1: Instalar Herramientas
1. **Visual Studio**: Descargar desde https://visualstudio.microsoft.com/
   - Instalar "Desktop development with C++"

2. **WinFlexBison**: 
   - Descargar desde: https://github.com/lexxmark/winflexbison/releases
   - Extraer a `C:\winflexbison`
   - Agregar `C:\winflexbison` al PATH

3. **SQLite**:
   - Descargar precompiled binaries desde: https://www.sqlite.org/download.html
   - Descargar "sqlite-amalgamation" y "sqlite-dll"
   - Extraer a `C:\sqlite3`

### Paso 2: Configurar Variables de Entorno
```powershell
# En PowerShell (como Administrador)
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\winflexbison;C:\sqlite3", "Machine")
```

### Paso 3: Compilar
```powershell
cd C:\Users\sebar\Documents\Repositories\fashion-ops-dsl\bison-con-bd

# Generar archivos
win_bison -d fashion-ops-bison.y
win_flex fashion-ops-dsl.l

# Compilar con Visual Studio Developer Command Prompt
cl fashion-ops-bison.tab.c lex.yy.c /I C:\sqlite3 /link C:\sqlite3\sqlite3.lib
```

---

## Opción 3: Usar Cygwin

### Paso 1: Instalar Cygwin
1. Descargar desde: https://www.cygwin.com/
2. Durante la instalación, seleccionar los siguientes paquetes:
   - gcc-core
   - bison
   - flex
   - libsqlite3-devel
   - make

### Paso 2: Compilar
```bash
cd /cygdrive/c/Users/sebar/Documents/Repositories/fashion-ops-dsl/bison-con-bd

bison -d fashion-ops-bison.y
flex fashion-ops-dsl.l
gcc -o fashion-ops.exe fashion-ops-bison.tab.c lex.yy.c -lsqlite3
```

---

## Verificar Instalación

Después de instalar las herramientas, verificar que estén disponibles:

```powershell
# Verificar versiones
gcc --version
bison --version
flex --version

# Verificar SQLite
sqlite3 --version
```

---

## Solución de Problemas Comunes

### Error: "sqlite3.h: No such file or directory"
**Solución**: Asegurarse de que SQLite3 está instalado y accesible:
```bash
# MSYS2
pacman -S mingw-w64-x86_64-sqlite3

# O especificar ruta manualmente
gcc -I/path/to/sqlite3/include -L/path/to/sqlite3/lib ...
```

### Error: "bison: command not found"
**Solución**: Agregar bison al PATH o usar la ruta completa:
```powershell
# Agregar MSYS2 al PATH de Windows
C:\msys64\mingw64\bin
```

### Error: "undefined reference to sqlite3_open"
**Solución**: Asegurarse de enlazar con `-lsqlite3`:
```bash
gcc ... -lsqlite3
```

### Error al ejecutar: "missing DLL"
**Solución**: Copiar los DLLs necesarios al directorio del ejecutable:
```bash
# Desde MSYS2
cp /mingw64/bin/libsqlite3-0.dll .
cp /mingw64/bin/libgcc_s_seh-1.dll .
cp /mingw64/bin/libwinpthread-1.dll .
```

---

## Estructura de Archivos Generados

Después de compilar, tendrás:
```
bison-con-bd/
├── fashion-ops-bison.y          # Archivo fuente Bison
├── fashion-ops-dsl.l            # Archivo fuente Flex
├── fashion-ops-bison.tab.c      # Generado por Bison
├── fashion-ops-bison.tab.h      # Generado por Bison
├── lex.yy.c                     # Generado por Flex
├── fashion-ops.exe              # Ejecutable compilado
├── fashion_ops.db               # Base de datos SQLite (se crea al ejecutar)
├── test.dsl                     # Archivo de prueba
└── README.md                    # Documentación
```

---

## Recursos Adicionales

- **Documentación Bison**: https://www.gnu.org/software/bison/manual/
- **Documentación Flex**: https://westes.github.io/flex/manual/
- **Documentación SQLite**: https://www.sqlite.org/docs.html
- **MSYS2**: https://www.msys2.org/
- **Tutorial Bison/Flex**: https://aquamentus.com/flex_bison.html
