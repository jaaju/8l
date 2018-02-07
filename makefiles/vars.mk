ifeq (,$(MAKECMDGOALS))
EFFECTIVE_GOAL = .DEFAULT_GOAL
else
EFFECTIVE_GOAL = $(MAKECMDGOALS)
endif

OBJDIR ?= build
STDLIB_PATH = $(shell readlink -f $(shell dirname $(shell $(CXX) --print-file-name libstdc++.so)))
override LDFLAGS += -Wl,-rpath=$(STDLIB_PATH)

OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))
DEPENDS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(SOURCES))

ROOT_PATH = $(shell dirname $(abspath $(firstword $(MAKEFILE_LIST))))

export CPPFLAGS CXXFLAGS LDFLAGS LDLIBS LIBS OBJDIR EFFECTIVE_GOAL STDLIB_PATH
