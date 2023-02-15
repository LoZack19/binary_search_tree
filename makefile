SH = /bin/bash

CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99
CFLAGS += -I./
CFLAGS += -g

OBJ = main.o
OBJ += src/bst_node.o src/bst.o
TARGET = main

.PHONY=all
all: $(TARGET)


%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY=clean
clean:
	rm $(OBJ)
	rm $(TARGET)

.PHONY=run
run:
	./$(TARGET)

.PHONY=debug
debug:
	gdb -q ./$(TARGET)

.PHONY=analyze
analyze:
	valgrind --tool=memcheck --leak-check=full ./$(TARGET)

.PHONY=count
count:
	wc -l *.c src/*.c include/*.h
