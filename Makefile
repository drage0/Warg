# Project
# Warg
#
# 28. 11. 2019.
#
MANAGARMR_MAKEDIR=./Managarmr
SKOLL_MAKEDIR=./Skoll
BIN_SYMLINK=./r

all: lua program

# Compile only the game itself (Managarmr)
#
program:
	cd $(MANAGARMR_MAKEDIR) && $(MAKE) all

# Compile only the Lua library (Skoll)
#
lua:
	cd $(SKOLL_MAKEDIR) && $(MAKE) linux

# Clean the build files.
# Call the clean Makefile target for both the program and the Lua library.
#
clean:
	cd $(MANAGARMR_MAKEDIR) && $(MAKE) clean
	cd $(SKOLL_MAKEDIR)     && $(MAKE) clean

# Preapre the software for running.
# Create a shortcut for the final binary. (BIN_SYMLINK)
#
install:
	ln -sfr $(MANAGARMR_MAKEDIR)/out/managarmr $(BIN_SYMLINK)

run:
	$(BIN_SYMLINK)

.PHONY: lua program clean all install run
