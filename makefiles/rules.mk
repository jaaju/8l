target: $(OBJDIR)/$(TARGET)

$(OBJDIR)/$(TARGET):: $(OBJECTS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) $(LIBS)

$(OBJDIR)/%.d: %.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) -MT $(OBJDIR)/$(subst .cpp,.o,$<) -MM -MF $@ $(CPPFLAGS) $(CXXFLAGS) $(SYS_INCLUDE_DIRS) $<

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

run:: target
	$(RUN_ENV) $(RUN_COMMAND) $(RUN_ARGS)

ifneq (,$(TEST_DIR))
test::
	$(MAKE) -I $(ROOT_PATH)/makefiles -C $(TEST_DIR) run
endif

ifdef COVERAGE_SCOPE
coverage::
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -fprofile-arcs -ftest-coverage" OBJDIR=$(OBJDIR)/.coverage RUN=$(RUN) run
ifneq (,$(TEST_DIR))
# There is a test suite. Run it to get coverage.
# Unless it is explicitly excluded.
ifneq (0,$(RUN_TESTS_FOR_COVERAGE))
	$(MAKE) -I $(ROOT_PATH)/makefiles -C $(TEST_DIR) coverage
endif
endif
	$(GENINFO) -o $(OBJDIR)/.coverage/coverage.all --gcov-tool $(GCOV) $(COVERAGE_SCOPE)
	$(LCOV) --gcov-tool $(GCOV) --no-external -o $(OBJDIR)/.coverage/coverage --extract $(OBJDIR)/.coverage/coverage.all "$(COVERAGE_SCOPE)/*"
	echo COVERAGE_EXCL is $(COVERAGE_EXCL)
ifneq (,$(COVERAGE_EXCL))
	$(LCOV) --gcov-tool $(GCOV) --no-external -o $(OBJDIR)/.coverage/coverage.final --remove $(OBJDIR)/.coverage/coverage $(COVERAGE_EXCL)
	mv $(OBJDIR)/.coverage/coverage.final $(OBJDIR)/.coverage/coverage
endif
	$(GENHTML) -o $(OBJDIR)/.coverage/report $(OBJDIR)/.coverage/coverage
	$(LCOV) --list $(OBJDIR)/.coverage/coverage 2>/dev/null
	$(LCOV) --gcov-tool $(GCOV) --summary $(OBJDIR)/.coverage/coverage.all "$(COVERAGE_SCOPE)/*" 2>&1 | grep -E '^\s*(lines|functions)\.'
endif

clean::
	rm -rf $(OBJECTS) $(DEPENDS) $(OBJDIR)/$(TARGET) $(OBJDIR)/.coverage
ifneq (,$(TEST_DIR))
	$(MAKE) -I $(ROOT_PATH)/makefiles -C $(TEST_DIR) clean
endif
# Set 'RM_OBJDIR = 1' if $(OBJDIR) can be deleted.
ifeq (1,$(RM_OBJDIR))
	rm -rf $(OBJDIR)
endif

ifneq (,$(filter-out clean,$(EFFECTIVE_GOAL)))
-include $(DEPENDS)
endif

# INCLUDE_MAKEFILES: set list of other makefiles to include, if any for customization.
-include $(INCLUDE_MAKEFILES)
