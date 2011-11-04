
BUILD = debug

.SECONDEXPANSION:

LIB_FILE = wtclasses.so
LIB = ./$(BUILD)/$(LIB_FILE)
LIB_PATH = $(LIB)

CXX = g++
LINK = g++
LIBS += -lboost_signals -lboost_regex -lboost_system -lboost_thread
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

