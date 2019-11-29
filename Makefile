# Project
# Mánagarmr
#
# 28. 11. 2019.
#
SOURCE_DIR=./src
OUTPUT_DIR=./out
BUILD_DIR=./build
DATA_DIR=./data
LIBS=-llua5.3 -lSDL2
CC=clang
CFLAGS=-g

all: prepare $(BUILD_DIR)/main.o $(OUTPUT_DIR)/managarmr

$(OUTPUT_DIR)/managarmr: $(BUILD_DIR)/main.o
	clang $(BUILD_DIR)/main.o -o $(OUTPUT_DIR)/managarmr $(LIBS)

$(BUILD_DIR)/main.o: $(SOURCE_DIR)/main.c $(SOURCE_DIR)/lua.h
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/main.c -o $(BUILD_DIR)/main.o

.PHONY: prepare
prepare:
	mkdir -pv $(OUTPUT_DIR) $(BUILD_DIR)
	ln -svf ../$(DATA_DIR) $(OUTPUT_DIR)/

.PHONY: clean
clean:
	-rm -rvf $(OUTPUT_DIR)/* $(BUILD_DIR)/*
