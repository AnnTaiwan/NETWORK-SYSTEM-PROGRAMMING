# 1. Compiler and Flag Settings
CC          := gcc
# Use -Wall -Wextra for strict warnings, -O2 for optimization, and -g for debugging
CFLAGS      := -Wall -Wextra -O2 #-g
# CPPFLAGS are for the Preprocessor. 
# Added -I../lib so the compiler can find "tlpi_hdr.h" and "get_num.h"
CPPFLAGS    := -I. -I../lib
LDFLAGS     :=
# Define the source files from the library directory that contain 
# the implementations of functions like getInt() and errExit()
# LIB_SRCS    := ../lib/get_num.c ../lib/error_functions.c
LIB_SRCS    := $(wildcard ../lib/*.c)

# 2. Source and Target Discovery
# Find all .c files in the current directory (e.g., free_and_sbrk.c)
SRCS        := $(wildcard *.c)
# Transform the list of .c files into executable names (strip the .c extension)
TARGETS     := $(SRCS:.c=)

# 3. Main Rules
.PHONY: all clean

# 'all' is the default target, building every program found in the directory
all: $(TARGETS)

# 4. Pattern Rule for Multiple Targets
# This rule tells make: "To create a target (like 'free_and_sbrk'), 
# compile 'free_and_sbrk.c' AND link it with the library source files."
# $@ = Target name (e.g., free_and_sbrk)
# $< = The first prerequisite (e.g., free_and_sbrk.c)
%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< $(LIB_SRCS) -o $@

# 5. Cleanup Utility
# Removes all generated executables to keep the workspace tidy
clean:
	rm -f $(TARGETS)
