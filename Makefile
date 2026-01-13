CFLAGS=-Wall -Wextra -Werror -std=c23 -g
CC=gcc
TARGET=main
SRCS= $(wildcard *.c)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

run:
	./$(TARGET)

clean:
	rm main
