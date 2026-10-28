# Project-wide warning interface target.
# Do NOT add -Werror yet: third-party headers and early experiments will fight you.
# Turn it on in week 3 once the codebase is stable.
add_library(agenticpp_warnings INTERFACE)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(agenticpp_warnings INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wnull-dereference
    -Wdouble-promotion
    -Wimplicit-fallthrough
    # Deliberately OFF for now; enable when comfortable (PLAN.md S5):
    # -Wconversion -Wsign-conversion
    # -Werror
  )
endif()
