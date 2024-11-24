# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Shell program source files
SRCS1 = exercise_49_1.c
SRCS2 = posix_msq_queue_file_client.c
SRCS3 = posix_msq_queue_file_server.c
# Object files
OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
OBJS3 = $(SRCS3:.c=.o)
# Executables
EXE1 = exercise_49_1
EXE2 = posix_msq_queue_file_client
EXE3 = posix_msq_queue_file_server
#header
HEAD = posix_msq_queue_file.h
# Default target
all: $(EXE1) $(EXE2) $(EXE3)

# Link executable from object files
$(EXE1): $(OBJS1)
	$(CC) $(CFLAGS) -o $@ $(OBJS1)

$(EXE2): $(OBJS2) 
	$(CC) $(CFLAGS) -o $@ $(OBJS2)

$(EXE3): $(OBJS3) 
	$(CC) $(CFLAGS) -o $@ $(OBJS3)
	
%.o: %.c $(HEAD)
	$(CC) $(CFLAGS) -c $<

# Clean up
clean:
	rm -f $(EXE1) $(EXE2) $(EXE3) $(OBJS1) $(OBJS2) $(OBJS3)

.PHONY: all clean

