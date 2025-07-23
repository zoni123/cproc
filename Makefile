.PHONY: clean

CC := gcc
CFLAGS := -Wall -Wextra

SRC_DIR := src
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

TARGET := cproc

build:
	$(CC) -o $(TARGET) $(SRC_FILES) $(CFLAGS)
clean:
	rm -f *.o $(TARGET)
