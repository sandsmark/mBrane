############# Names and dirs #############

PROGNAME = mBrane

BINDIR = ../bin
SRCDIR = .
BUILDDIR = build

INCLUDEDIRS = . ../Core

LIBRARIES = ../bin/arch/libCore.so

############# Include system stuff #############

include ../Makefile.sys

############# Files to compile #############

CPPFILES = executing.cpp main.cpp messaging.cpp module_descriptor.cpp networking.cpp
CPPFILES += network_id.cpp node.cpp space.cpp 

############# Setup dirs #############

PROGRAM = $(BINDESTDIR)/$(PROGNAME)

SRCFILES = $(patsubst %.cpp,$(SRCDIR)/%.cpp,$(CPPFILES))
OBJFILES = $(patsubst %.cpp,$(BUILDDIR)/%.o,$(CPPFILES))

INCLUDES = $(patsubst %,-I% ,$(INCLUDEDIRS))
LIBS = $(subst /arch/,/$(ARCH)/,$(LIBRARIES))

RESULT := $(shell if [ ! -d $(BINDIR) ]; then mkdir $(BINDIR); fi)
RESULT := $(shell if [ ! -d $(BINDESTDIR) ]; then mkdir $(BINDESTDIR); fi)
RESULT := $(shell if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR); fi)
RESULT := $(shell if [ ! -d $(BUILDDIR)/thirdparty ]; then mkdir $(BUILDDIR)/thirdparty; fi)

############# Overall commands #############

all : $(PROGRAM)
	@echo $(PROGRAM) ready

debug: all

profile: all

############# Dependencies #############

$(BUILDDIR)/%.d: $(SRCDIR)/%.cpp
	@echo Making dependencies for $(<F)...
	@$(SHELL) -ec '$(CC) -MM $(INCLUDES) -D $(SYSTEM) $< \
		| sed '\''s/\($(*F)\)\.o[ :]*/\1.o $(@F) : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),totalclean)
  -include $(OBJFILES:.o=.d)
endif
endif

############# Compiling #############

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@echo Compiling $(<F)
	$(CC) $(DEBUG) $(INCLUDES) $(STANDARD) $(NOTLINK) $(OUTPUT_OPTION) $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo Compiling $(<F)
	@gcc $(DEBUG) $(INCLUDES) -Wall -w -Wstrict-prototypes -Wmissing-prototypes -O2 -o $@ -c $<

############# Linking #############

$(PROGRAM): $(OBJFILES)
	@echo Linking $(PROGNAME) $(SYSTEM)...
	$(CC) $(DEBUG) $(STANDARD) $(OBJFILES) $(LIBRARY) $(LIBPATHS) -o $(PROGRAM) $(LIBS) $(OTHERLIBS)

############# Cleaning #############

clean:
	@echo Removing build and binary files
	@$(RMALL) $(BUILDDIR)
	@$(RMALL) $(BINDIR)

totalclean : clean
	@echo Removing Visual Studio temporary files
	@$(RMALL) *.suo *.ncb

############# Additional commands #############

rebuild : clean all

run : all
	@echo Running $(PROGNAME)
	@echo
	@$(PROGNAME)
