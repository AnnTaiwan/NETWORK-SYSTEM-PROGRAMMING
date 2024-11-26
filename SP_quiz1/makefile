# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Shell program source files
SRCS1 = prob1.c
SRCS2 = prob2.c
# Object files
OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
# Executables
EXE1 = prob1
EXE2 = prob2

# Default target
all: $(EXE1) $(EXE2)

# Link executable from object files
$(EXE1): $(OBJS1)
	$(CC) $(CFLAGS) -o $@ $(OBJS1)

$(EXE2): $(OBJS2)
	$(CC) $(CFLAGS) -o $@ $(OBJS2)
	
%.o: %.c 
	$(CC) $(CFLAGS) -c $<

# Clean up
clean:
	rm -f $(EXE1) $(OBJS1) $(EXE2) $(OBJS2)

.PHONY: all clean

