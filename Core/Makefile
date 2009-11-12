############# Names and dirs #############

LIBNAME = Core.a

LIBDIR = lib
SRCDIR = .
BUILDDIR = build

INCLUDEDIRS = .

LIBS = 

############# Include system stuff #############

include ../Makefile.sys

############# Files to compile #############

CPPFILES = class_register.cpp control_messages.cpp mdaemon_node.cpp
CPPFILES += memory.cpp message.cpp module.cpp module_node.cpp module_register.cpp
CPPFILES += network_interface.cpp object.cpp payload.cpp utils.cpp
CPPFILES += xml_parser.cpp

############# Setup dirs #############

LIBRARY = $(LIBDESTDIR)/$(LIBNAME)

SRCFILES = $(patsubst %.cpp,$(SRCDIR)/%.cpp,$(CPPFILES))
OBJFILES = $(patsubst %.cpp,$(BUILDDIR)/%.o,$(CPPFILES))

INCLUDES = $(patsubst %,-I% ,$(INCLUDEDIRS))
LIBS = $(subst /arch/,/$(ARCH)/,$(LIBRARIES))

RESULT := $(shell if [ ! -d $(LIBDIR) ]; then mkdir $(LIBDIR); fi)
RESULT := $(shell if [ ! -d $(LIBDESTDIR) ]; then mkdir $(LIBDESTDIR); fi)
RESULT := $(shell if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR); fi)
RESULT := $(shell if [ ! -d $(BUILDDIR)/thirdparty ]; then mkdir $(BUILDDIR)/thirdparty; fi)

############# Overall commands #############

all : $(LIBRARY)
	@echo $(LIBRARY) ready

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
	@$(CC) $(DEBUG) $(INCLUDES) $(STANDARD) $(NOTLINK) $(OUTPUT_OPTION) $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo Compiling $(<F)
	@gcc $(DEBUG) $(INCLUDES) -Wall -w -Wstrict-prototypes -Wmissing-prototypes -O2 -o $@ -c $<
	
############# Linking #############

$(LIBRARY): $(OBJFILES)
	@echo Creating $(LIBRARY)...
	@$(RM) -f $(LIBRARY)
	@$(AR) $(ARFLAGS) $(LIBRARY) $(OBJFILES)
	@$(RANLIB) $(LIBRARY)

############# Cleaning #############

clean:
	@echo Removing build and binary files
	@$(RMALL) $(BUILDDIR)
	@$(RMALL) $(LIBDIR)

totalclean : clean
	@echo Removing Visual Studio temporary files
	@$(RMALL) *.suo *.ncb

############# Additional commands #############

rebuild : clean all
