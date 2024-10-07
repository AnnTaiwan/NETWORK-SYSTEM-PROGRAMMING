# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Source files
SRCS = my_tail.c

# Object files (replace .c with .o for each source file)
OBJS = $(SRCS:.c=.o)

# Executable names (replace .c with no extension for each source file)
EXES = $(SRCS:.c=)

# Default target
all: $(EXES)

# Generic rule to compile each executable
$(EXES): %: %.o
	$(CC) $(CFLAGS) -o $@ $<

# Generic rule to compile each object file
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Clean up the project
clean:
	rm -f $(EXES) $(OBJS)

.PHONY: all clean
