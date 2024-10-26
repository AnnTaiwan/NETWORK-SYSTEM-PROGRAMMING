# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Shell program source files
SRCS = $(wildcard *.c)  # Automatically get all .c files in the current directory
# Object files
OBJS = $(SRCS:.c=.o)     # Convert .c file names to .o file names
# Executables
EXES = $(SRCS:.c=)       # Generate executable names by removing .c extension from source files

# Default target
all: $(EXES)

# Link executable from object files
%: %.o                # This line defines how to create an executable from its .o file
	$(CC) $(CFLAGS) -o $@ $<

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Clean up
clean:
	rm -f $(EXES) $(OBJS)  # Remove executables and object files

.PHONY: all clean        # Specify that 'all' and 'clean' are not files

