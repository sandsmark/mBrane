############# Names and dirs #############

BINLIBNAME = Core
PROGNAME = CoreTest

BINDIR = ../bin
SRCDIR = .
BUILDDIR = build

INCLUDEDIRS = .

LIBS = 

############# Include system stuff #############

include ../Makefile.sys

############# Files to compile #############

CPPFILES = class_register.cpp control_messages.cpp mdaemon_node.cpp
CPPFILES += message.cpp module.cpp module_node.cpp module_register.cpp
CPPFILES += network_interface.cpp object.cpp payload.cpp utils.cpp
CPPFILES += xml_parser.cpp

PROG_CPPFILES = CoreTest.cpp

############# Setup dirs #############

PROGRAM = $(BINDESTDIR)/$(PROGNAME)
BINLIBRARY = $(BINDESTDIR)/lib$(BINLIBNAME).so

SRCFILES = $(patsubst %.cpp,$(SRCDIR)/%.cpp,$(CPPFILES))
OBJFILES = $(patsubst %.cpp,$(BUILDDIR)/%.o,$(CPPFILES))

PROG_SRCFILES = $(patsubst %.cpp,$(SRCDIR)/%.cpp,$(PROG_CPPFILES))
PROG_OBJFILES = $(patsubst %.cpp,$(BUILDDIR)/%.o,$(PROG_CPPFILES))

INCLUDES = $(patsubst %,-I% ,$(INCLUDEDIRS))
LIBS = $(subst /arch/,/$(ARCH)/,$(LIBRARIES))

RESULT := $(shell if [ ! -d $(BINDIR) ]; then mkdir $(BINDIR); fi)
RESULT := $(shell if [ ! -d $(BINDESTDIR) ]; then mkdir $(BINDESTDIR); fi)
RESULT := $(shell if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR); fi)

############# Overall commands #############

all : $(BINLIBRARY) $(PROGRAM)
	@echo $(BINLIBRARY) and $(PROGRAM) ready

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
	@$(CC) $(DEBUG) $(INCLUDES) $(STANDARD) $(LIBCOPT) $(NOTLINK) $(OUTPUT_OPTION) $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo Compiling $(<F)
	@gcc $(DEBUG) $(INCLUDES) $(LIBCOPT) -Wall -w -Wstrict-prototypes -Wmissing-prototypes -O2 -o $@ -c $<

############# Linking #############

$(PROGRAM): $(LIBRARY) $(PROG_OBJFILES)
	@echo Linking $(PROGNAME) $(SYSTEM)...
	@$(CC) $(DEBUG) $(STANDARD) $(PROG_OBJFILES) $(LIBRARY) $(LIBPATHS) -o $(PROGRAM) $(LIBS) $(BINLIBRARY) $(OTHERLIBS)
	
$(BINLIBRARY): $(OBJFILES)
	@echo Linking $(BINLIBRARY) $(SYSTEM)...
	@$(CC) $(DEBUG) $(STANDARD) $(LIBOPT) $(OBJFILES) $(LIBPATHS) -o $(BINLIBRARY) $(LIBS) $(OTHERLIBS)

############# Cleaning #############

clean:
	@echo Removing build and binary files
	@$(RMALL) $(BUILDDIR)
	@$(RMALL) $(BINLIBRARY) $(PROGRAM)

totalclean : clean
	@echo Removing Visual Studio temporary files
	@$(RMALL) *.suo *.ncb

############# Additional commands #############

rebuild : clean all
