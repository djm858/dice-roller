# Declaration of variables
SRC_DIR := src
INCLUDE_DIR := include
CC = gcc -I$(INCLUDE_DIR)
CC_FLAGS = -w -Werror -Wall -Wpedantic -std=c99
LD_FLAGS = -lm

# File names
EXEC = roll
# Incorporates all the files with .c extension
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(LD_FLAGS)
	
# To obtain object files
%.0: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
