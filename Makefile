include InstallDirs.inc

name = libwtclasses

BUILD = debug
VERSION = $(shell cat VERSION)
SHORT_VERSION = $(shell echo $(VERSION) | sed 's@\.[0-9]\+$$@@')
fullname = $(name)-$(VERSION)

ifneq (,$(findstring $(MAKECMDGOALS),install deb))
BUILD = release
endif

.SECONDEXPANSION:

DYNAMIC_LIB_SHORT = $(name).so
DYNAMIC_LIB_SHORT_PATH = ./$(BUILD)/$(name).so
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

examples_cpp = $(wildcard examples/*.cpp)
examples_binaries = $(examples_cpp:.cpp=.wt)

css = $(wildcard css/*.css)
pys = locales-test.py
locales = $(wildcard locales/wtclasses*.xml)
project_files = Doxyfile InstallDirs.inc LICENSE Makefile VERSION

dist_files = $(sources) $(headers) $(project_files) $(css) $(locales) \
	$(pys) $(examples_cpp)
dist_dir = $(fullname)
dist_tar = $(fullname).tar.gz

.PHONY: build
build: $$(DYNAMIC_LIB_PATH) $$(STATIC_LIB_PATH) $$(DYNAMIC_LIB_SHORT_PATH)

$(BUILD)/%.o: src/$$*.cpp $$(headers)
	mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(DYNAMIC_LIB_PATH): $$(objects)
	mkdir -p $(dir $@)
	$(LINK) $(LFLAGS) $(LIBS) $(objects) -o $@

$(DYNAMIC_LIB_SHORT_PATH):
	mkdir -p $(dir $@)
	ln -f -s $(DYNAMIC_LIB) $@

$(STATIC_LIB_PATH): $$(objects)
	mkdir -p $(dir $@)
	ar -cvq $@ $^

.PHONY: doc
doc:
	$(MAKE) install-buildless DESTDIR=./doc-source prefix=/usr
	doxygen

.PHONY: installdirs
installdirs:
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) -d $(DESTDIR)$(includedir)$(includesubdir)
	$(INSTALL) -d $(DESTDIR)$(bindir)

.PHONY: install-buildless
install-buildless: $(headers) locales-test.py installdirs
	$(INSTALL) $(headers) $(DESTDIR)$(includedir)$(includesubdir)
	$(INSTALL) locales-test.py $(DESTDIR)$(bindir)/locales-test

.PHONY: install
install: build install-buildless installdirs
	$(INSTALL) $(DYNAMIC_LIB_PATH) $(DESTDIR)$(libdir)
	$(INSTALL) $(DYNAMIC_LIB_SHORT_PATH) $(DESTDIR)$(libdir)
	$(INSTALL) $(STATIC_LIB_PATH) $(DESTDIR)$(libdir)

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
	cd $(dist_dir) && yes | dh_make -l -p $(name)_$(VERSION)
	cp -flr debian/* $(dist_dir)/debian
	sed 's@SHORT_VERSION@$(SHORT_VERSION)@g' \
		< debian/control.in > $(dist_dir)/debian/control
	cp -fl debian/libwtclasses.SHORT_VERSION.install.in \
		$(dist_dir)/debian/libwtclasses.$(SHORT_VERSION).install
	cd $(dist_dir)/debian/ && rm *.in
	cd $(dist_dir) && dpkg-buildpackage

.PHONY: check
check: locales

.PHONY: locales
locales: locales-test.py
	./locales-test.py --prefix=wc --sections wbi

.PHONY: examples
examples: $$(examples_binaries)

%.wt: %.cpp
ifeq (,$(EXAMPLES_SYSTEM_LIB))
	$(MAKE) install DESTDIR=. prefix=/usr
	$(CXX) -L./usr/lib -I./usr/include \
		$(LIBS) -lwtclasses -lwthttp $< -o $@
else
	$(CXX) $(LIBS) -lwtclasses -lwthttp $< -o $@
endif

.PHONY: clean
clean:
	rm -fr debug release usr libwtclasses* doc* examples/*.wt

