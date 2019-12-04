# Project
# Mánagarmr
#
# 28. 11. 2019.
#
SOURCE_DIR=./src
OUTPUT_DIR=./out
BUILD_DIR=./build
DATA_DIR=./data
LUA_DIR=./Skoll

all: prepare lua program

program:
	cd $(SOURCE_DIR) && $(MAKE) all

lua:
	cd $(LUA_DIR) && $(MAKE) linux

prepare:
	mkdir -pv $(OUTPUT_DIR) $(BUILD_DIR)
	ln -svf ../$(DATA_DIR) $(OUTPUT_DIR)/

clean:
	cd $(SOURCE_DIR) && $(MAKE) clean
	cd $(LUA_DIR)    && $(MAKE) clean

.PHONY: lua program clean prepare all
