############# Names and dirs #############

BINDIRS  = Core TCP UDP mBrane

BINDIR = bin

############# Include system stuff #############

include Makefile.sys

############# Setup dirs #############

SUBDIRS = $(BINDIRS)

RESULT := $(shell if [ ! -d $(BINDIR) ]; then mkdir $(BINDIR); fi)
RESULT := $(shell if [ ! -d $(BINDESTDIR) ]; then mkdir $(BINDESTDIR); fi)

############# Overall commands #############

all: compile

debug: compile

profile: compile

compile:
	@for dir in $(SUBDIRS); do \
		if cd $(TOP_DIR)/$$dir; then make $(MAKECMDGOALS) --no-print-directory; fi;\
		cd $(TOP_DIR);\
	done

############# Cleaning up #############

clean:
	@for dir in $(SUBDIRS); do \
		if cd $(TOP_DIR)/$$dir; then echo Cleaning $$dir; make clean --no-print-directory; fi;\
		cd $(TOP_DIR);\
	done

totalclean:
	@for dir in $(SUBDIRS); do \
		if cd $(TOP_DIR)/$$dir; then echo Total cleaning $$dir; make totalclean --no-print-directory; fi;\
		cd $(TOP_DIR);\
	done

rebuild: clean all
