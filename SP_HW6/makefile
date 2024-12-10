# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Shell program source files
SRCS1 = exercise44_1.c
SRCS2 = exercise44_2.c
SRCS3 = popen_pclose.c
SRCS4 = temp44_2.c
# Object files
OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
OBJS3 = $(SRCS3:.c=.o)

OBJS4 = $(SRCS4:.c=.o)
# Executables
EXE1 = exercise44_1
EXE2 = exercise44_2

EXE4 = temp44_2
#header
HEAD = popen_pclose.h
# Default target
all: $(EXE1) $(EXE2) $(EXE4)

# Link executable from object files
$(EXE1): $(OBJS1)
	$(CC) $(CFLAGS) -o $@ $(OBJS1)

$(EXE2): $(OBJS2) $(OBJS3) $(HEAD)
	$(CC) $(CFLAGS) -o $@ $(OBJS2) $(OBJS3)
	
$(EXE4): $(OBJS4) $(OBJS3) $(HEAD)
	$(CC) $(CFLAGS) -o $@ $(OBJS4) $(OBJS3)
	
%.o: %.c $(HEAD)
	$(CC) $(CFLAGS) -c $<

# Clean up
clean:
	rm -f $(EXE1) $(EXE2) $(EXE4) $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4)

.PHONY: all clean

