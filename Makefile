CC = cc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11
SRCS = $(wildcard src/*.c)
EXEC = ashley

.PHONY: all clean

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(EXEC)

clean:
	rm -rf $(EXEC)
