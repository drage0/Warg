# Makefile for installing Lua
# See doc/readme.html for installation and customization instructions.

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= none

# == END OF USER SETTINGS -- NO NEED TO CHANGE ANYTHING BELOW THIS LINE =======

# Convenience platforms targets.
PLATS= aix bsd c89 freebsd generic linux macosx mingw posix solaris

# Lua version and release.
V= 5.3
R= $V.4

# Targets start here.
all:	$(PLAT)

$(PLATS) clean:
	@printf '\e[34m'
	cd src && $(MAKE) $@
	@printf '\e[39m'

none:
	@echo "Please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"
	@echo "See doc/readme.html for complete instructions."

# echo config parameters
echo:
	@cd src && $(MAKE) -s echo
	@echo "PLAT= $(PLAT)"
	@echo "V= $V"
	@echo "R= $R"

# echo pkg-config data
pc:
	@echo "version=$R"

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all $(PLATS) clean test local none dummy echo pecho lecho

# (end of Makefile)
