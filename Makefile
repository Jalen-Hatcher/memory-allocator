CFLAGS:=-Wall -Wextra -Werror -std=c23 -g
CC:=gcc
TARGET:=build/main

SRC_DIRS:=src src/Allocator src/Application
INC_DIRS:=src/Allocator src/Application src/Utilities
SRCS:=$(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))

INC_DIRS_FLAGS:=$(foreach dir, $(INC_DIRS), -I./$(dir))

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INC_DIRS_FLAGS) -o $@ $^

run:
	make && ./$(TARGET)

clean:
	rm build/main
