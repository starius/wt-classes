include InstallDirs.inc
include RelPath.inc

name = libwtclasses

BUILD = debug
VERSION = $(shell cat VERSION)
SONAME = $(shell cat SONAME)
fullname = $(name)-$(VERSION)

ifneq (,$(findstring $(MAKECMDGOALS),install install-lib deb))
BUILD = release
endif

.SECONDEXPANSION:

DYNAMIC_LIB_SHORT = $(name).so
DYNAMIC_LIB_SONAME = $(DYNAMIC_LIB_SHORT).$(SONAME)
DYNAMIC_LIB = $(DYNAMIC_LIB_SHORT).$(VERSION)
DYNAMIC_LIB_PATH = ./$(BUILD)/$(DYNAMIC_LIB)
STATIC_LIB = $(name).a
STATIC_LIB_PATH = ./$(BUILD)/$(STATIC_LIB)

CXX = g++
LINK = g++
LIBS += -lboost_signals
LIBS += -lwt
CXXFLAGS += -pipe -Wall -W -fPIC
CXXFLAGS += -I$(BUILD) -Isrc
LFLAGS += -shared
ifeq ($(BUILD), debug)
CXXFLAGS += -g -O0 -DDEBUG
LFLAGS += -O0
else
CXXFLAGS += -O3 -DNDEBUG
LFLAGS += -O3
endif
INSTALL = install

sources = $(sort $(wildcard src/*.cpp) $(wildcard src/*/*.cpp))
headers = $(sort $(wildcard src/*.hpp) $(wildcard src/*/*.hpp))
objects = $(subst src/,$(BUILD)/,$(sources:.cpp=.o))

includesubdir = /Wt/Wc

examples_cpp = $(wildcard examples/*.cpp)
examples_binaries = $(examples_cpp:.cpp=.wt)

css = $(wildcard css/*.css)
pys = locales-test.py
locales = $(wildcard locales/wtclasses*.xml)
project_files = Doxyfile.in *.inc LICENSE Makefile VERSION SONAME \
	Changelog
man_rests = locales-test.1.rst
mans = $(man_rests:.rst=)

dist_files = $(sources) $(headers) $(project_files) $(css) $(locales) \
	$(pys) $(examples_cpp) $(man_rests)
dist_dir = $(fullname)
dist_tar = $(fullname).tar.gz

.PHONY: build
build: build-lib examples

.PHONY: build-lib
build-lib: $$(DYNAMIC_LIB_PATH) $$(STATIC_LIB_PATH)

$(BUILD)/%.o: src/$$*.cpp $$(headers)
	mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(DYNAMIC_LIB_PATH): $$(objects)
	mkdir -p $(dir $@)
	$(LINK) $(LFLAGS) $(LIBS) $(objects) -Wl,-soname,$(DYNAMIC_LIB_SONAME) -o $@

$(STATIC_LIB_PATH): $$(objects)
	mkdir -p $(dir $@)
	ar rcs $@ $^

.PHONY: doc
doc: locales-test.1 Doxyfile
	$(MAKE) install-buildless DESTDIR=./doc-source prefix=/usr
	doxygen

Doxyfile: Doxyfile.in
	sed 's@{PROJECT_NUMBER}@$(VERSION)@g' < $< > $@

.PHONY: installdirs
installdirs:
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) -d $(DESTDIR)$(includedir)$(includesubdir)
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) -d $(DESTDIR)$(datadir)/Wt/Wc/locales
	$(INSTALL) -d $(DESTDIR)$(datadir)/Wt/resources/Wc/css
	$(INSTALL) -d $(DESTDIR)$(mandir)/man1

.PHONY: install-buildless
install-buildless: $(headers) locales-test.py $$(mans) installdirs
	$(INSTALL) -m 644 $(headers) $(DESTDIR)$(includedir)$(includesubdir)
	$(INSTALL) locales-test.py $(DESTDIR)$(bindir)/locales-test
	$(INSTALL) -m 644 locales-test.1 $(DESTDIR)$(mandir)/man1/
	$(INSTALL) -m 644 $(locales) $(DESTDIR)$(datadir)/Wt/Wc/locales/
	$(INSTALL) -m 644 $(css) $(DESTDIR)$(datadir)/Wt/resources/Wc/css/

.PHONY: install-lib
install-lib: build-lib install-buildless installdirs
	$(INSTALL) -m 644 $(DYNAMIC_LIB_PATH) $(DESTDIR)$(libdir)
	ln -f -s $(DYNAMIC_LIB_SONAME) $(DESTDIR)$(libdir)/$(DYNAMIC_LIB_SHORT)
	ln -f -s $(DYNAMIC_LIB) $(DESTDIR)$(libdir)/$(DYNAMIC_LIB_SONAME)
	$(INSTALL) -m 644 $(STATIC_LIB_PATH) $(DESTDIR)$(libdir)

.PHONY: install
install: install-lib install-buildless installdirs
	$(INSTALL) $(examples_binaries) $(DESTDIR)$(bindir)

.PHONY: dist
dist: $$(dist_files)
	mkdir -p $(dist_dir)
	cp --parents $^ $(dist_dir)
	tar -czf $(dist_tar) $(addprefix $(dist_dir)/, $(dist_files))

.PHONY: deb
deb:
	$(MAKE) dist
	cp -fl $(dist_tar) $(name)_$(VERSION).orig.tar.gz
	rm -rf $(dist_dir)/debian
	mkdir -p $(dist_dir)/debian
	cp -flr debian/* $(dist_dir)/debian
	sed 's@SONAME@$(SONAME)@g' \
		< debian/control.in > $(dist_dir)/debian/control
	cp -fl debian/libwtclassesSONAME.install.in \
		$(dist_dir)/debian/libwtclasses$(SONAME).install
	cd $(dist_dir)/debian/ && rm -f *.in
	cd $(dist_dir) && debuild

.PHONY: check
check: locales

.PHONY: locales
locales: locales-test.py
	./locales-test.py --prefix=wc --sections wbi

.PHONY: examples
examples: $$(examples_binaries)

%.wt: %.cpp
ifeq (,$(EXAMPLES_SYSTEM_LIB))
	$(MAKE) install-lib DESTDIR=. prefix=/usr
	$(CXX) -L./usr/lib -I./usr/include \
		$(LIBS) -lwtclasses -lwthttp $< -o $@
else
	$(CXX) $(LIBS) -lwtclasses -lwthttp $< -o $@
endif

.PHONY: clean
clean:
	rm -fr debug release usr libwtclasses* doc* examples/*.wt

locales-test.1: locales-test.1.rst
	rst2man $< > $@

