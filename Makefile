
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o , $(SRC))

TARGET = ksh

$(TARGET) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^


obj/%.o : src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@


clean: 
	rm -rf obj/ $(TARGET)

.PHONY: clean
