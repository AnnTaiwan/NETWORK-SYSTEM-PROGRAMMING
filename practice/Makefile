# 1. Compiler and Flag Settings
CC          := gcc
CFLAGS      := -Wall -Wextra -O2 -g
CPPFLAGS    := -I.
LDFLAGS     := 
LDLIBS      := 

# 2. Source and Target Discovery
# Find all .c files in the directory
SRCS        := $(wildcard *.c)

# Create a list of targets by stripping the .c extension
# e.g., main.c -> main, test_mkdir.c -> test_mkdir
TARGETS     := $(SRCS:.c=)

# 3. Main Rules
.PHONY: all clean

# 'all' now depends on every single target found
all: $(TARGETS)

# 4. Pattern Rule for Multiple Targets
# This tells make: "To create a target (like 'test1'), look for 'test1.c'"
# $@ is the target name (e.g., test1)
# $< is the source file (e.g., test1.c)
%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@

# 5. Cleanup Utility
clean:
	rm -f $(TARGETS)
