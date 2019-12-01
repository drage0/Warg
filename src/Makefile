# Project
# Mánagarmr
#
# 1. 12. 2019.
#
OUTPUT_DIR=../out
BUILD_DIR=../build
DATA_DIR=./data
LUA_DIR=./Skoll
LIBS=-llua -lSDL2
CC=clang
CFLAGS=-g


all:  $(BUILD_DIR)/main.o $(OUTPUT_DIR)/managarmr

$(OUTPUT_DIR)/managarmr: $(BUILD_DIR)/main.o
	$(CC) $(BUILD_DIR)/main.o -o $(OUTPUT_DIR)/managarmr $(LIBS)

$(BUILD_DIR)/main.o: main.c lua.h
	$(CC) $(CFLAGS) -c main.c -o $(BUILD_DIR)/main.o

clean:
	-rm -rvf $(OUTPUT_DIR)/* $(BUILD_DIR)/*

.PHONY: lua clean prepare all
