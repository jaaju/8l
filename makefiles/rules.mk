$(TARGET):: $(OBJECTS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) $(LIBS)

$(OBJDIR)/%.d: %.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) -MT $(OBJDIR)/$(subst .cpp,.o,$<) -MM -MF $@ $(CPPFLAGS) $(CXXFLAGS) $(SYS_INCLUDE_DIRS) $<

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

ifneq (,$(TEST_DIR))
test::
	$(MAKE) -I $(ROOT_PATH)/makefiles -C $(TEST_DIR)
endif

clean::
	rm -rf $(OBJECTS) $(DEPENDS) $(TARGET)
ifneq (,$(TEST_DIR))
	$(MAKE) -I $(ROOT_PATH)/makefiles -C $(TEST_DIR) clean
endif

ifneq (,$(filter-out clean,$(EFFECTIVE_GOAL)))
-include $(DEPENDS)
endif
