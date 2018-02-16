QUOTE = '
# fix syntax highlighting! '

ifeq (,$(MAKECMDGOALS))
EFFECTIVE_GOAL = .DEFAULT_GOAL
else
EFFECTIVE_GOAL = $(MAKECMDGOALS)
endif

# To override these, set them in ${HOME}/.make.defaults
CXX ?= $(shell which g++)
GCOV ?= $(shell which gcov)
LCOV ?= $(shell which lcov)
GENHTML ?= $(shell which genhtml)
GENINFO ?= $(shell which geninfo)

OBJDIR ?= build
STDLIB_PATH = $(shell readlink -f $(shell dirname $(shell $(CXX) --print-file-name libstdc++.so)))
override LDFLAGS += -Wl,-rpath=$(STDLIB_PATH)

OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))
DEPENDS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(SOURCES))

ROOT_PATH = $(shell dirname $(abspath $(firstword $(MAKEFILE_LIST))))

# The executable to invoke for the 'run' recipe.
# Set this to run something, e.g. a test script, different from the target that is built.
ifdef RUN
RUN_COMMAND = $(RUN)
else
RUN_COMMAND = $(OBJDIR)/$(TARGET)
endif

# Set to define the shell env (list of name=value variables) under which $(RUN_COMMAND) should be run.
RUN_ENV ?=

# Set to command-line argument that should be passed to $(RUN_COMMAND).
RUN_ARGS ?=

# COVERAGE_SCOPE: Set this to a directory path. All source files in this path will be measured for coverage.
# Setting this variable also provides a 'coverage' recipe.
# The 'coverage' recipe does the following:
# 	- Run the 'run' recipe,
# 	- Run the 'run' recipe of the TEST_DIR subproject (unless that is turned off by setting RUN_TESTS_FOR_COVERAGE=0),
# 	- Generate coverage report in the directory '$(OBJDIR)/.coverage/report'.

# EXCLUDE_FROM_COVERAGE: list all shell wildcard patterns that should be excluded from coverage measurement.
ifdef EXCLUDE_FROM_COVERAGE
COVERAGE_EXCL = $(foreach EXCL,$(EXCLUDE_FROM_COVERAGE),$(QUOTE)$(EXCL)$(QUOTE))
endif

# Exclude tests code from coverage scope.
ifneq (,$(TEST_DIR))
COVERAGE_EXCL += $(QUOTE)$(CURDIR)/$(TEST_DIR)/*$(QUOTE)
endif

export CPPFLAGS CXXFLAGS LDFLAGS LDLIBS LIBS OBJDIR EFFECTIVE_GOAL STDLIB_PATH
export TARGET RUN_ENV RUN_COMMAND RUN_ARGS
export LCOV GCOV COVERAGE_SCOPE
