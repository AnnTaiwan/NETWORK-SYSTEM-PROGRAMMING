# Compiler
CC = gcc

# Compiler flags
CFLAGS = -g -pthread

# Shell program source files
SRCS1 = prob1.c

SRCS2 = prob2.c
SRCS3 = stack.c

# Object files
OBJS1 = $(SRCS1:.c=.o)

OBJS2 = $(SRCS2:.c=.o)
OBJS3 = $(SRCS3:.c=.o)

# Executables
EXE1 = prob1

EXE2 = prob2
#header
HEAD1 = stack.h
# Default target
all: $(EXE1) $(EXE2) 

# Link executable from object files
$(EXE1): $(OBJS1)
	$(CC) $(CFLAGS) -o $@ $(OBJS1)

$(EXE2): $(OBJS2) $(OBJS3) 
	$(CC) $(CFLAGS) -o $@ $(OBJS2) $(OBJS3)


	
%.o: %.c $(HEAD1)
	$(CC) $(CFLAGS) -c $<

# Clean up
clean:
	rm -f $(EXE1) $(EXE2) $(OBJS1) $(OBJS2) $(OBJS3)

.PHONY: all clean

