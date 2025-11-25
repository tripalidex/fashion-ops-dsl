# Makefile para FashionOpsDSL con SQLite

CC = gcc
CFLAGS = -Wall -g
LIBS = -lsqlite3

# Archivos
BISON_FILE = fashion-ops-bison.y
LEX_FILE = fashion-ops-dsl.l
BISON_OUT = fashion-ops-bison.tab.c
BISON_HDR = fashion-ops-bison.tab.h
LEX_OUT = lex.yy.c
EXECUTABLE = fashion-ops

# Regla principal
all: $(EXECUTABLE)

# Generar el parser con Bison
$(BISON_OUT) $(BISON_HDR): $(BISON_FILE)
	bison -d $(BISON_FILE)

# Generar el lexer con Flex
$(LEX_OUT): $(LEX_FILE) $(BISON_HDR)
	flex $(LEX_FILE)

# Compilar el ejecutable
$(EXECUTABLE): $(BISON_OUT) $(LEX_OUT)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(BISON_OUT) $(LEX_OUT) $(LIBS)

# Limpiar archivos generados
clean:
	rm -f $(BISON_OUT) $(BISON_HDR) $(LEX_OUT) $(EXECUTABLE) fashion_ops.db

# Probar con un archivo de ejemplo
test: $(EXECUTABLE)
	./$(EXECUTABLE) test.dsl

.PHONY: all clean test
