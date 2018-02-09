# This assumes the compiler and boost libraries are available in default locations.
# If not, set variables CXX, CPPFLAGS, LDFLAGS, etc. in ~/.make.defaults
-include $(shell echo $${HOME})/.make.defaults

TARGET = test-8l
SOURCES = test.cpp core/test.cpp common/client.cpp

override CPPFLAGS += -I. -I..

run: $(TARGET)
	./$(TARGET)

include vars.mk
include rules.mk
