PREFIX      := /usr/local
INSTALL_DIR := $(PREFIX)/include

.DEFAULT_GOAL := all
.PHONY        := all install uninstall

MKDIR := mkdir --parents
RMDIR := rmdir --parents --ignore-fail-on-non-empty

all:
	@echo 'This is a only a C/C++ header, there is nothing to compile ;)'
	@echo
	@echo 'Type “make install” to install at the default location (“'"$(INSTALL_DIR)"'”).'
	@echo 'Type “make INSTALL_DIR=/some/directory” to install where you want.'
	@echo
	@echo 'Similarly, type “make uninstall” or “make INSTALL_DIR=/some/directory uninstall” to uninstall.'

install:
	$(MKDIR) -- $(INSTALL_DIR)
	cp -f -- include/contracts.h $(INSTALL_DIR)

uninstall:
	$(RM) -- $(INSTALL_DIR)/contracts.h
	$(RMDIR) -- $(INSTALL_DIR)
