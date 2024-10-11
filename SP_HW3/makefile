# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Shell program source files
SRCS = shell.c parse.c builtin.c run_command.c is_background.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable
EXES = shell

# Headers
HEADER = shell.h

# Default target
all: $(EXES)

# Link executable from object files
$(EXES): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
    
# Compile .c to .o, ensuring header dependencies are tracked
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $<

# Clean up
clean:
	rm -f $(EXES) $(OBJS)

.PHONY: all clean

