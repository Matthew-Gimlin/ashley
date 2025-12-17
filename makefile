CC = cc
CFLAGS = -g -O0 -std=c11 -Wall -Wextra -Wpedantic -Iinclude

APP = ashley
APP_SRC = src/main.c
APP_OBJ = $(APP_SRC:.c=.o)

LIB = libashley.a
LIB_SRC = src/api.c \
		  src/common.c \
		  src/lex.c \
		  src/parse.c \
		  src/debug.c
LIB_OBJ = $(LIB_SRC:.c=.o)

.PHONY: all clean

all: $(APP) $(LIB)

$(LIB): $(LIB_OBJ)
	ar rcs $@ $^

$(APP): $(APP_OBJ) $(LIB)
	$(CC) $(CFLAGS) -o $@ $(APP_OBJ) $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(APP) $(LIB) $(APP_OBJ) $(LIB_OBJ)
