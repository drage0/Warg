# Project
# Mánagarmr
#
# 28. 11. 2019.
#
MANAGARMR_MAKEDIR=./Managarmr
SKOLL_MAKEDIR=./Skoll

all: lua program

program:
	cd $(MANAGARMR_MAKEDIR) && $(MAKE) all

lua:
	cd $(SKOLL_MAKEDIR) && $(MAKE) linux

clean:
	cd $(MANAGARMR_MAKEDIR) && $(MAKE) clean
	cd $(SKOLL_MAKEDIR)    && $(MAKE) clean

.PHONY: lua program clean all
