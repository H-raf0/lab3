CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lm

SRC = code.c mt19937ar.c
OBJ = $(SRC:.c=.o)
DEPS = mt19937ar.h
EXEC = my_program

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
