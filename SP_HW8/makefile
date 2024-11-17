# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Shell program source files
SRCS1 = fifo_seqnum_client.c
SRCS2 = misbehaving_client.c
SRCS3 = fifo_seqnum_server_nonblock.c

SRCS4 = exercise44_7_server.c
SRCS5 = exercise44_7_client.c
# Object files
OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
OBJS3 = $(SRCS3:.c=.o)

OBJS4 = $(SRCS4:.c=.o)
OBJS5 = $(SRCS5:.c=.o)
# Executables
EXE1 = fifo_seqnum_client
EXE2 = misbehaving_client
EXE3 = fifo_seqnum_server_nonblock

EXE4 = exercise44_7_server
EXE5 = exercise44_7_client
#header
HEAD = fifo_seqnum.h
# Default target
all: $(EXE1) $(EXE2) $(EXE3) $(EXE4) $(EXE5)

# Link executable from object files
$(EXE1): $(OBJS1) 
	$(CC) $(CFLAGS) -o $@ $(OBJS1)
$(EXE2): $(OBJS2) 
	$(CC) $(CFLAGS) -o $@ $(OBJS2) 
$(EXE3): $(OBJS3) 
	$(CC) $(CFLAGS) -o $@ $(OBJS3)	
$(EXE4): $(OBJS4) 
	$(CC) $(CFLAGS) -o $@ $(OBJS4)
$(EXE5): $(OBJS5) 
	$(CC) $(CFLAGS) -o $@ $(OBJS5)
%.o: %.c $(HEAD)
	$(CC) $(CFLAGS) -c $<

# Clean up
clean:
	rm -f $(EXE1) $(EXE2) $(EXE3) $(EXE4) $(EXE5) $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) 

.PHONY: all clean

