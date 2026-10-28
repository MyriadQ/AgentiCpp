add_executable(unit_tests
  smoke_test.cpp
  # one file per module, added as it is written:
  # test_line_buffer.cpp
  # test_template_parser.cpp
  # test_vectorizer.cpp
  # test_memory_store.cpp
  # test_policy.cpp
  # test_executor.cpp
  # test_agent.cpp
)

target_link_libraries(unit_tests PRIVATE agenticpp_core)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)

# Catch2 registers each TEST_CASE as a separate ctest entry, so a failure
# names the behaviour that broke.
include(Catch)
catch_discover_tests(unit_tests)
