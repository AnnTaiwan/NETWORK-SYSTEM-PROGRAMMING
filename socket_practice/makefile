# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Automatically detect all .c files in the current directory
SRCS = $(wildcard *.c)

# Generate .o files from .c files
OBJS = $(SRCS:.c=.o)

# Generate executable names from source file names
EXES = $(SRCS:.c=)

# Default target (compile all executables)
all: $(EXES)

# Compile each source file to an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into executables
%: %.o
	$(CC) $(CFLAGS) -o $@ $<

# Clean up
clean:
	rm -f $(EXES) $(OBJS)

.PHONY: all clean

