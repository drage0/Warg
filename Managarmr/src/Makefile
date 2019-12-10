# Project
# Mánagarmr
#
# 1. 12. 2019.
#
CC=clang
CFLAGS=-g

SKOLL_DIR=../../Skoll/out
OUTPUT_DIR=../out
BUILD_DIR=../build
LIBS=-L$(SKOLL_DIR) -llua -lSDL2 -lm -ldl


all: $(BUILD_DIR)/main.o $(OUTPUT_DIR)/managarmr

$(OUTPUT_DIR)/managarmr: $(BUILD_DIR)/main.o
	$(CC) $(BUILD_DIR)/main.o -o $(OUTPUT_DIR)/managarmr $(LIBS)

$(BUILD_DIR)/main.o: main.c lua.h utility.h
	$(CC) $(CFLAGS) -c main.c -o $(BUILD_DIR)/main.o

clean:
	-rm -rvf $(OUTPUT_DIR)/* $(BUILD_DIR)/*

.PHONY: clean all
