# Directories
INC = include
OUT = output
LIB = lib

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11

# Source files
SRCS = example.c $(LIB)/openai.c $(INC)/cJSON.c
OBJS = $(SRCS:.c=.o)

# Output executable
TARGET = test

# Libraries
LIBS = -lcurl

# Build rules
all: create_dir $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(OUT)/$(TARGET) $(OBJS) $(LIBS)
	rm -rf */*.o
	rm -rf *.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

create_dir:
	mkdir -p $(OUT)

run:
	./$(OUT)/$(TARGET)

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# PHONY targets to prevent conflicts with files named 'clean' or 'all'
.PHONY: all create_dir clean
.SILENT: run