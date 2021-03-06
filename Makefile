#=====================================================================#
#                        Dynamic Parser Library                       #
# Makefile.in                                                         #
# This is the Makefile-template for DPL.                              #
# If the Makefile has been lost, copy this file to Makefile and type  #
# make config. This will generate a Makefile that is configured for   #
# your system. You can then use                                       #
# make [clean | config | build | install | uninstall].                #
# The standard use is:                                                 #
# make config; make build; sudo make install[; sudo make uninstall].  #
# - Lasse Nielsen.                                                    #
#=====================================================================#
name = dpl
version = 2020
libname = lib$(name).so
#OS_LINUXlibname = lib$(name).so
#OS_MAClibname = lib$(name).dylib
#OS_LINUXlibversion = .$(version)
#OS_MAClibversion =
COMMENT = OS_
OS_AUTO = $(shell uname -s)

compiler = g++
ctags = ctags
opt = -O3
args = -std=c++0x -fPIC $(opt) -I./include/
#OS_MAClibs = 
#OS_LINUXlibs = -lrcp

library_objects = \
  objects/tokenizer.o \
  objects/parsetree.o \
  objects/bnf.o \
  objects/parser.o \
  objects/symbnf.o \
  objects/symparser.o \
  objects/slrparser.o

default:
	@echo "Use make config, make build, sudo make install, make clean and if you don't like it sudo make uninstall."

all: config build install clean

phony: default config build install uninstall clean tags package deb doc doc/html

doc: doc/html

doc/html: include/$(name)/*.hpp source/*.cpp Doxyfile
	mkdir -p doc/
	doxygen Doxyfile

config:
ifeq ($(OS),linux)
	sed -e "s/#$(COMMENT)LINUX//g" Makefile.in > Makefile
else
ifeq ($(OS),osx)
	sed -e "s/#$(COMMENT)MAC//g" Makefile.in > Makefile
else
ifeq ($(OS_AUTO),Linux)
	sed -e "s/#$(COMMENT)LINUX//g" Makefile.in > Makefile
else
ifeq ($(OS_AUTO),Darwin)
	sed -e "s/#$(COMMENT)MAC//g" Makefile.in > Makefile
else
	@echo "Unable to recognize os $(OS_AUTO)."
	@echo "If linux is used please run"
	@echo "OS=linux make config."
	@echo "If OS X is used please run"
	@echo "OS=osx make config."
endif
endif
endif
endif

build: $(libname)$(libversion)

include/$(name)/config.hpp:
	@echo "Creating config header"
	@echo "#ifndef CONFIG_DPL" > include/$(name)/config.hpp
	@echo "#define CONFIG_DPL" >> include/$(name)/config.hpp
	@echo "#include <string>" >> include/$(name)/config.hpp
#OS_MAC	@echo "#define OS_X" >> include/$(name)/config.hpp
#OS_LINUX	@echo "#define OS_LINUX" >> include/$(name)/config.hpp
	@echo "#endif" >> include/$(name)/config.hpp

install: $(libname)$(libversion)
	@echo "Copying library"
	cp $(libname)$(libversion) /usr/lib/
#OS_LINUX	ln -f -s /usr/lib/$(libname)$(libversion) /usr/lib/$(libname)
	@echo "Copying include-files"
	mkdir -p /usr/include/$(name)
	cp include/$(name)/*.hpp /usr/include/$(name)/
	chmod -R a+rx /usr/include/$(name)
#OS_LINUX	@echo "Reindexing libraries"
#OS_LINUX	ldconfig -n /usr/lib

uninstall:
	@echo "Removing library"
	rm -f /usr/lib/$(libname)*
	@echo "Removing include-files"
	rm -Rf /usr/include/$(name)
#OS_LINUX	@echo "Reindexing libraries"
#OS_LINUX	ldconfig -n /usr/lib

clean:
	touch clean~
	touch packages
	touch debs
	touch include/$(name)/config.hpp
	touch $(libname)$(libversion)
	rm *~
	rm -Rf packages
	rm -Rf debs
	rm -Rf objects
	rm include/$(name)/config.hpp
	rm $(libname)$(libversion)
	cp Makefile.in Makefile

package:
	git clone . ./$(name)-$(version)
	rm -Rf ./$(name)-$(version)/.git
	mkdir -p packages/
	tar -czf packages/$(name)-$(version).tgz $(name)-$(version)
	rm -Rf $(name)-$(version)

deb: $(libname)$(libversion)
	echo "Making DEB package"
	mkdir -p debs/
	echo "Making data folder"
	mkdir -p debs/lib$(name)_$(version)_i386
	mkdir -p debs/lib$(name)_$(version)_i386/usr
	mkdir -p debs/lib$(name)_$(version)_i386/usr/lib
	cp $(libname)$(libversion) debs/lib$(name)_$(version)_i386/usr/lib/
	mkdir -p debs/lib$(name)_$(version)_i386/usr/include
	cp -R include/$(name) debs/lib$(name)_$(version)_i386/usr/include/$(name)
	echo "Making control"
	mkdir -p debs/lib$(name)_$(version)_i386/DEBIAN
	echo "Package: libdpl"                                         > debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Version: $(version)"                                    >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Architecture: i386"                                     >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Maintainer: Lasse Nielsen <lasse.nielsen.dk@gmail.com>" >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Installed-Size: 1024"                                   >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Pre-Depends: dpkg (>= 1.14.12ubuntu3)"                  >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Depends: librcp (>= 1.5)"                               >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Recommends: apims"                                      >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Suggests: apims"                                        >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Conflicts: "                                            >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Replaces: "                                             >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Provides: libdpl"                                       >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Section: library"                                       >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Priority: optional"                                     >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Homepage: http://www.thelas.dk"                         >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "Description: Library for quickly writing parsers."      >> debs/lib$(name)_$(version)_i386/DEBIAN/control
	echo "#!/bin/sh"                                               > debs/lib$(name)_$(version)_i386/DEBIAN/postinst
	echo "ldconfig -n /usr/lib"                                   >> debs/lib$(name)_$(version)_i386/DEBIAN/postinst
	echo "ln -f -s /usr/lib/$(libname)$(libversion) /usr/lib/$(libname)" >> debs/lib$(name)_$(version)_i386/DEBIAN/postinst
	chmod a+x debs/lib$(name)_$(version)_i386/DEBIAN/postinst
	echo "#!/bin/sh"                                               > debs/lib$(name)_$(version)_i386/DEBIAN/postrm
	echo "ldconfig -n /usr/lib"                                   >> debs/lib$(name)_$(version)_i386/DEBIAN/postrm
	chmod a+x debs/lib$(name)_$(version)_i386/DEBIAN/postrm
	echo "Finalizing package"
	dpkg -b debs/lib$(name)_$(version)_i386/
	rm -Rf debs/lib$(name)_$(version)_i386

$(libname)$(libversion): $(library_objects)
#OS_LINUX	$(compiler) -shared -Wl,-soname,$(libname).1 -o $(libname)$(libversion) $(library_objects) $(libs)
#OS_MAC	$(compiler) -dynamiclib -o $(libname) $(library_objects) $(libs)

objects/%.o: source/%.cpp include/$(name)/*.hpp  include/$(name)/config.hpp
	mkdir -p objects
	$(compiler) -c source/$*.cpp $(args) -o objects/$*.o

tags: $(name)/*.hpp $(name)/*.cpp
	$(ctags) -a -o ~/.ctags $(PWD)/include/$(name)/*.hpp $(PWD)/source/*.cpp

