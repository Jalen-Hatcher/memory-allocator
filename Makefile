CFLAGS=-Wall -Wextra -Werror -std=c23 -g
CC=gcc
TARGET=build/main
SRCS= $(wildcard src/*.c)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

run:
	make && ./$(TARGET)

clean:
	rm build/main
