
BUILD = debug
VERSION = $(shell cat VERSION)

ifneq (,$(findstring $(MAKECMDGOALS),dist deb))
BUILD = release
endif

.SECONDEXPANSION:

LIB_FILE = wtclasses.so
LIB = ./$(BUILD)/$(LIB_FILE)
LIB_PATH = $(LIB)

dist_files = $(LIB)
dist_dir = wtclasses-$(VERSION)
dist_tgz = $(dist_dir).tar.gz
dist_install_dir = /usr/lib

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

.PHONY: build
build: $$(LIB)

include $(makefiles)

$(BUILD)/%.d: $$(tosource)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -MM | sed 's,.\+\.o[ :]*,$(@:.d=.o) $@ : ,' > $@;

$(BUILD)/%.o: $$(precompiled) $$(downloaded)
	$(CXX) -c $(CXXFLAGS) $(tosource) -o $@

$(LIB): $$(sources) $$(headers) $$(makefiles) $$(objects)
	mkdir -p $(dir $@)
	$(LINK) $(LFLAGS) $(LIBS) $(objects) -o $@

.PHONY: doc
doc:
	doxygen

.PHONY: dist
dist: $$(dist_files)
	mkdir -p $(dist_dir)$(dist_install_dir)
	cp -f -l $(LIB) $(dist_dir)$(dist_install_dir)/lib$(LIB_FILE).$(VERSION)
	tar --exclude=debian -czf $(dist_tgz) $(dist_dir)

.PHONY: deb
deb: dist
	cp -fl $(dist_tgz) wtclasses_$(VERSION).orig.tar.gz
	rm -rf $(dist_dir)/debian
	cd $(dist_dir) && yes | dh_make -l
	cp -flr debian/* $(dist_dir)/debian
	cd $(dist_dir) && dpkg-buildpackage

