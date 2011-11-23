include InstallDirs.inc

name = libwtclasses

BUILD = debug
VERSION = $(shell cat VERSION)

ifneq (,$(findstring $(MAKECMDGOALS),dist install deb))
BUILD = release
endif

.SECONDEXPANSION:

DYNAMIC_LIB = $(name).so
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
CXXFLAGS += -g -O0
LFLAGS += -O0
else
CXXFLAGS += -O3
LFLAGS += -O3
endif
INSTALL = install

sources = $(sort $(wildcard src/*.cpp) $(wildcard src/*/*.cpp))
headers = $(sort $(wildcard src/*.hpp) $(wildcard src/*/*.hpp))
objects = $(subst src/,$(BUILD)/,$(sources:.cpp=.o))

includesubdir = /Wt/Wc

dist_files = $(DYNAMIC_LIB_PATH) $(STATIC_LIB_PATH) $(headers)
dist_dir = $(name)
dist_tgz = $(dist_dir).tar.gz
dist_header_dir = $(includedir)$(includesubdir)

examples_cpp = $(wildcard examples/*.cpp)
examples_binaries = $(examples_cpp:.cpp=.wt)

.PHONY: build
build: $$(DYNAMIC_LIB_PATH) $$(STATIC_LIB_PATH)

$(BUILD)/%.o: src/$$*.cpp $$(headers)
	mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(DYNAMIC_LIB_PATH): $$(objects)
	mkdir -p $(dir $@)
	$(LINK) $(LFLAGS) $(LIBS) $(objects) -o $@

$(STATIC_LIB_PATH): $$(objects)
	mkdir -p $(dir $@)
	ar -cvq $@ $^

.PHONY: doc
doc: dist
	doxygen

.PHONY: install
install: $(DYNAMIC_LIB_PATH) $(STATIC_LIB_PATH) $(headers)
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) $(DYNAMIC_LIB_PATH) $(DESTDIR)$(libdir)/$(DYNAMIC_LIB).$(VERSION)
	ln -f -s $(DYNAMIC_LIB).$(VERSION) $(DESTDIR)$(libdir)/$(DYNAMIC_LIB)
	$(INSTALL) $(STATIC_LIB_PATH) $(DESTDIR)$(libdir)
	$(INSTALL) -d $(DESTDIR)$(includedir)$(includesubdir)
	$(INSTALL) $(headers) $(DESTDIR)$(includedir)$(includesubdir)
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) locales-test.py $(DESTDIR)$(bindir)/locales-test

.PHONY: dist
dist: $$(dist_files) build
	mkdir -p $(dist_dir)$(libdir)
	cp -f -l $(DYNAMIC_LIB_PATH) $(dist_dir)$(libdir)/$(DYNAMIC_LIB).$(VERSION)
	ln -f -s $(DYNAMIC_LIB).$(VERSION) $(dist_dir)$(libdir)/$(DYNAMIC_LIB)
	cp -f -l $(STATIC_LIB_PATH) $(dist_dir)$(libdir)
	mkdir -p $(dist_dir)$(dist_header_dir)
	cp -f -l $(headers) $(dist_dir)$(dist_header_dir)
	mkdir -p $(dist_dir)$(bindir)
	cp -f -l locales-test.py $(dist_dir)$(bindir)/locales-test
	tar --exclude=debian -czf $(dist_tgz) $(dist_dir)

.PHONY: deb
deb:
	$(MAKE) dist prefix=/usr
	cp -fl $(dist_tgz) $(name)_$(VERSION).orig.tar.gz
	rm -rf $(dist_dir)/debian
	cd $(dist_dir) && yes | dh_make -l -p $(name)_$(VERSION)
	cp -flr debian/* $(dist_dir)/debian
	cd $(dist_dir) && dpkg-buildpackage

.PHONY: check
check: locales

.PHONY: locales
locales: locales-test.py
	./locales-test.py --prefix=wc --sections wbi

.PHONY: examples
examples: $$(examples_binaries)

%.wt: %.cpp dist
	$(CXX) -L$(dist_dir)$(libdir) -I$(dist_dir)$(includedir) $(LIBS) \
		-lwtclasses -lwthttp $< -o $@

