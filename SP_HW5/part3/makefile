# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Shell program source files
SRCS1 = convert.c
SRCS2 = lookup1.c main.c
# Object files
OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
# Executable
EXES1 = convert
EXES2 = main
# Headers
HEADER = dict.h

# Default target
all: $(EXES1) $(EXES2)

# Link executable from object files
$(EXES1): $(OBJS1)
	$(CC) $(CFLAGS) -o $@ $(OBJS1)
$(EXES2): $(OBJS2)
	$(CC) $(CFLAGS) -o $@ $(OBJS2)
# Compile .c to .o, ensuring header dependencies are tracked
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $<

# Clean up
clean:
	rm -f $(EXES1) $(OBJS1) $(EXES2) $(OBJS2)

.PHONY: all clean
