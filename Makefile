name = libwtclasses

BUILD = debug
VERSION = $(shell cat VERSION)

ifneq (,$(findstring $(MAKECMDGOALS),dist deb))
BUILD = release
endif

.SECONDEXPANSION:

LIB_FILE = $(name).so
LIB = ./$(BUILD)/$(LIB_FILE)
LIB_PATH = $(LIB)
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

sources = $(sort $(wildcard src/*.cpp) $(wildcard src/*/*.cpp))
headers = $(sort $(wildcard src/*.hpp) $(wildcard src/*/*.hpp))
objects = $(subst src/,$(BUILD)/,$(sources:.cpp=.o))
makefiles = $(objects:.o=.d)
tosource = src/$*.cpp

dist_files = $(LIB) $(STATIC_LIB_PATH) $(headers)
dist_dir = $(name)
dist_tgz = $(dist_dir).tar.gz
dist_install_dir = /usr/lib
dist_header_dir = /usr/include/Wt/Wc
bindir = /usr/bin

.PHONY: build
build: $$(LIB) $$(STATIC_LIB_PATH)

include $(makefiles)

$(BUILD)/%.d: $$(tosource)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -MM | sed 's,.\+\.o[ :]*,$(@:.d=.o) $@ : ,' > $@;

$(BUILD)/%.o: $$(precompiled) $$(downloaded)
	$(CXX) -c $(CXXFLAGS) $(tosource) -o $@

$(LIB): $$(sources) $$(headers) $$(makefiles) $$(objects)
	mkdir -p $(dir $@)
	$(LINK) $(LFLAGS) $(LIBS) $(objects) -o $@

$(STATIC_LIB_PATH): $$(objects)
	mkdir -p $(dir $@)
	ar -cvq $@ $^

.PHONY: doc
doc: dist
	doxygen

.PHONY: dist
dist: $$(dist_files) build
	mkdir -p $(dist_dir)$(dist_install_dir)
	cp -f -l $(LIB) $(dist_dir)$(dist_install_dir)/$(LIB_FILE).$(VERSION)
	ln -f -s $(LIB_FILE).$(VERSION) $(dist_dir)$(dist_install_dir)/$(LIB_FILE)
	cp -f -l $(STATIC_LIB_PATH) $(dist_dir)$(dist_install_dir)
	mkdir -p $(dist_dir)$(dist_header_dir)
	cp -f -l $(headers) $(dist_dir)$(dist_header_dir)
	mkdir -p $(dist_dir)$(bindir)
	cp -f -l locales-test.py $(dist_dir)$(bindir)/locales-test
	tar --exclude=debian -czf $(dist_tgz) $(dist_dir)

.PHONY: deb
deb: dist
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

