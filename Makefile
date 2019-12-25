# Project
# Mánagarmr
#
# 28. 11. 2019.
#
MANAGARMR_MAKEDIR=./Managarmr
SKOLL_MAKEDIR=./Skoll
RUN_SCRIPT=./r

all: lua program

program:
	cd $(MANAGARMR_MAKEDIR) && $(MAKE) all

lua:
	cd $(SKOLL_MAKEDIR) && $(MAKE) linux

clean:
	cd $(MANAGARMR_MAKEDIR) && $(MAKE) clean
	cd $(SKOLL_MAKEDIR)    && $(MAKE) clean

install:
	ln -sfr $(MANAGARMR_MAKEDIR)/out/managarmr $(RUN_SCRIPT)

run:
	$(RUN_SCRIPT)

.PHONY: lua program clean all install run

