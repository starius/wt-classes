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
LIBS += -lboost_signals -lboost_system
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
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644
RST2MAN = rst2man
HAVE_DOT = NO

sources = $(sort $(wildcard src/*.cpp) $(wildcard src/*/*.cpp))
headers = $(sort $(wildcard src/*.hpp) $(wildcard src/*/*.hpp))
objects = $(subst src/,$(BUILD)/,$(sources:.cpp=.o))

includesubdir = /Wt/Wc

examples_cpp = $(sort $(wildcard examples/*.cpp) examples/all.cpp)
examples_binaries = $(examples_cpp:.cpp=.wt)
examples_source = $(filter-out examples/all.cpp,$(examples_cpp))
exampleslibdir = $(libdir)/$(name)/examples
examplesdocdir = $(docdir)/examples
exampleslibreldir = $(call relpath,$(exampleslibdir),$(examplesdocdir))
referencelink = $(docdir)/reference
referencedir = $(datadir)/$(name)/reference

css = $(wildcard css/*.css)
js = $(sort $(wildcard js/*.js) js/jquery.countdown.js)
pys = locales-test.py examples/make-all.py
locales = $(wildcard locales/wtclasses*.xml)
project_files = Doxyfile.in *.inc LICENSE Makefile VERSION SONAME \
	Changelog AUTHORS README.rst NEWS INSTALL main.doc
man_rests = locales-test.1.rst
mans = $(man_rests:.rst=)
templates = examples/all.cpp.in

dist_files = $(sources) $(headers) $(project_files) $(css) $(js) $(locales) \
	$(pys) $(examples_source) $(man_rests) $(templates)
dist_dir = $(fullname)
dist_tar = $(fullname).tar.gz

doc_examples = examples/xxd-wt.cpp.ex examples/swfstore.cpp.ex \
	examples/gather.cpp.ex $(examples_cpp)

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
doc: locales-test.1 Doxyfile $$(doc_examples)
	$(MAKE) install-buildless DESTDIR=./doc-source prefix=/usr
	doxygen

Doxyfile: Doxyfile.in VERSION
	sed 's@{PROJECT_NUMBER}@$(VERSION)@g;s@{HAVE_DOT}@$(HAVE_DOT)@g' < $< > $@

include Install.inc

.PHONY: check
check: locales

.PHONY: locales
locales: locales-test.py
	./locales-test.py --prefix=wc --sections captcha common time wbi

.PHONY: examples
examples: $(DYNAMIC_LIB_PATH)
	$(MAKE) install-lib DESTDIR=. prefix=/usr
	$(MAKE) $(examples_binaries)

%.wt: %.cpp $(DYNAMIC_LIB_PATH)
ifeq (,$(EXAMPLES_SYSTEM_LIB))
	$(CXX) -L./usr/lib -I./usr/include \
		$(LIBS) -lwtclasses -lwthttp $< -o $@
else
	$(CXX) $(LIBS) -lwtclasses -lwthttp $< -o $@
endif

.PHONY: clean
clean:
	rm -fr debug release usr libwtclasses* doc* examples/*.wt

locales-test.1: locales-test.1.rst
	$(RST2MAN) $< > $@

examples/xxd-wt.cpp.ex: examples/xxd-wt.cpp
	sed '/bool validate/,/^}/d' < $< | \
		egrep -iv 'que|[/ ]\*|bits|ps|cols|valid|box|line|bind' | \
		sed '/^$$/N;/^\n$$/D' > $@

examples/swfstore.cpp.ex: examples/swfstore.cpp
	egrep -iv 'gather' $< > $@

examples/gather.cpp.ex: examples/swfstore.cpp
	egrep -iv 'bind|button|text|click|\<k\>' $< > $@

examples/all.cpp: $$(examples_source) examples/all.cpp.in
	sed '/int main/,/^}/d' $(examples_source) > $@
	cat examples/all.cpp.in | python examples/make-all.py >> $@

.SECONDARY: js/jquery.countdown.js
js/jquery.countdown.js:
	wget http://keith-wood.name/zip/jquery.countdown.package-1.5.11.zip -O c.zip
	unzip -o c.zip $(@F) -d $(@D)
	rm c.zip

