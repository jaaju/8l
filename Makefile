# This assumes the compiler and boost libraries are available in default locations.
# If not, set variables CXX, CPPFLAGS, LDFLAGS, etc. in ~/.make.defaults
-include $(shell echo $${HOME})/.make.defaults

OBJDIR = build

TARGET = 8l
SOURCES = 8l.cpp

override CPPFLAGS += -I.
override CXXFLAGS += -std=c++11 -Wall -Werror -g -ggdb -O0 -fno-inline -fno-eliminate-unused-debug-types

override LDFLAGS +=
override LDLIBS += -lboost_system
override LIBS +=

TEST_DIR = test

include makefiles/vars.mk
include makefiles/rules.mk
