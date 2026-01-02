CFLAGS=-Wall -Wextra -Werror -std=c23 -pedantic -ggdb
CC=gcc
TARGET=main

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $<

run:
	./$(TARGET)

clean:
	rm main
