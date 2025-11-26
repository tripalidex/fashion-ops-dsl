# Makefile para FashionOpsDSL con SQLite3

CC = gcc
CFLAGS = -Wall
LIBS = -lsqlite3
BISON = bison
FLEX = flex

TARGET = fashion-ops
BISON_SRC = fashion-ops-bison.y
FLEX_SRC = fashion-ops-dsl.l
BISON_OUT = fashion-ops-bison.tab.c fashion-ops-bison.tab.h
FLEX_OUT = lex.yy.c

all: $(TARGET)

$(TARGET): $(BISON_OUT) $(FLEX_OUT)
	$(CC) $(CFLAGS) -o $(TARGET) lex.yy.c fashion-ops-bison.tab.c $(LIBS)

$(BISON_OUT): $(BISON_SRC)
	$(BISON) -d $(BISON_SRC)

$(FLEX_OUT): $(FLEX_SRC) $(BISON_OUT)
	$(FLEX) $(FLEX_SRC)

clean:
	rm -f $(TARGET) $(BISON_OUT) $(FLEX_OUT) fashion_ops.db

test: $(TARGET)
	./$(TARGET) test.txt

.PHONY: all clean test
