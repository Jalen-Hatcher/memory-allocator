CFLAGS=-Wall -Wextra -Werror -std=c23 -pedantic -ggdb
CC=gcc
TARGET=main
SRCS= $(wildcard *.c)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

run:
	./$(TARGET)

clean:
	rm main
