# Compiler 
CC = gcc

# Compiler flags
CFLAGS = -Wall -g -pthread

# Target executable
TARGET = threaded_sum

# Source file
SRC = threaded_sum.c

# Object file
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

# Link object file to create the executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compile source file into object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJ)