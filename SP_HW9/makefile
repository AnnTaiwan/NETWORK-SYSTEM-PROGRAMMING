# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Shell program source files
SRCS1 = exercise_49_1.c

SRCS2 = posix_msq_queue_file_client.c
SRCS3 = posix_msq_queue_file_server.c

SRCS4 = posix_shm_writer.c
SRCS5 = posix_shm_reader.c
# Object files
OBJS1 = $(SRCS1:.c=.o)

OBJS2 = $(SRCS2:.c=.o)
OBJS3 = $(SRCS3:.c=.o)

OBJS4 = $(SRCS4:.c=.o)
OBJS5 = $(SRCS5:.c=.o)
# Executables
EXE1 = exercise_49_1

EXE2 = posix_msq_queue_file_client
EXE3 = posix_msq_queue_file_server

EXE4 = posix_shm_writer
EXE5 = posix_shm_reader
#header
HEAD1 = posix_msq_queue_file.h
HEAD2 = posix_shm.h
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
	
%.o: %.c $(HEAD1) $(HEAD2)
	$(CC) $(CFLAGS) -c $<

# Clean up
clean:
	rm -f $(EXE1) $(EXE2) $(EXE3) $(EXE4) $(EXE5) $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) 

.PHONY: all clean

