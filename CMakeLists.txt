cmake_minimum_required(VERSION 3.25)
project(agenticpp LANGUAGES CXX VERSION 0.1.0)

# ---------------------------------------------------------------------------
# Global settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)          # for clangd / editors
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(Warnings)

option(AGENTICPP_SANITIZE "Build with AddressSanitizer + UBSanitizer" OFF)
option(AGENTICPP_TSAN     "Build with ThreadSanitizer"                OFF)
option(AGENTICPP_NATIVE   "Benchmark-only -march=native; breaks scalar/AVX2 isolation" OFF)
option(AGENTICPP_ENABLE_LLM "Enable libcurl + simdjson (LLM transport)"          OFF)

if(AGENTICPP_SANITIZE AND AGENTICPP_TSAN)
  message(FATAL_ERROR "ASan and TSan cannot be combined. Use separate presets.")
endif()

if(AGENTICPP_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer
                      -fno-sanitize-recover=undefined)
  add_link_options(-fsanitize=address,undefined)
  message(STATUS "Sanitizers: ASan + UBSan")
endif()

if(AGENTICPP_TSAN)
  add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
  add_link_options(-fsanitize=thread)
  message(STATUS "Sanitizers: TSan")
endif()

if(AGENTICPP_NATIVE)
  message(WARNING "AGENTICPP_NATIVE compiles every TU with -march=native; do not use it for Table 2")
  add_compile_options(-march=native)
endif()

# ---------------------------------------------------------------------------
# Dependencies via FetchContent (pinned — see PLAN.md §1)
# ---------------------------------------------------------------------------
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.16.1
  GIT_SHALLOW    TRUE
  SYSTEM
)
FetchContent_MakeAvailable(Catch2)

# nlohmann is a core dependency: config parsing (Policy/AgentConfig) and JSON encoding.
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.12.0
  GIT_SHALLOW    TRUE
  SYSTEM
)
FetchContent_MakeAvailable(nlohmann_json)

# --- LLM transport deps. Flip with -DAGENTICPP_ENABLE_LLM=ON (day 8). ------
if(AGENTICPP_ENABLE_LLM)
  FetchContent_Declare(
    simdjson
    GIT_REPOSITORY https://github.com/simdjson/simdjson.git
    GIT_TAG        v5.0.5
    GIT_SHALLOW    TRUE
    SYSTEM
  )
  set(SIMDJSON_JUST_LIBRARY ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(simdjson)

  find_package(CURL REQUIRED)
  message(STATUS "LLM deps: simdjson, CURL ${CURL_VERSION_STRING}")
endif()

# ---------------------------------------------------------------------------
# Core library: everything testable lives here
# ---------------------------------------------------------------------------
file(GLOB AGENTICPP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# The concrete HTTP client is only compiled when the LLM dependencies are
# available. The interface header stays available to tests and the Agent.
if(NOT AGENTICPP_ENABLE_LLM)
  list(FILTER AGENTICPP_SOURCES EXCLUDE REGEX ".*/deepseek_client\\.cpp$")
endif()

add_library(agenticpp_core STATIC ${AGENTICPP_SOURCES})
target_include_directories(agenticpp_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(agenticpp_core PRIVATE agenticpp_warnings)
target_link_libraries(agenticpp_core PUBLIC nlohmann_json::nlohmann_json)
target_compile_features(agenticpp_core PUBLIC cxx_std_20)

if(AGENTICPP_ENABLE_LLM)
  target_compile_definitions(agenticpp_core PUBLIC AGENTICPP_ENABLE_LLM=1)
  target_link_libraries(agenticpp_core PRIVATE simdjson::simdjson CURL::libcurl)
else()
  target_compile_definitions(agenticpp_core PUBLIC AGENTICPP_ENABLE_LLM=0)
endif()

# ---------------------------------------------------------------------------
# Executables
# ---------------------------------------------------------------------------
add_executable(agentd apps/agentd.cpp)
target_link_libraries(agentd PRIVATE agenticpp_core)

# ---------------------------------------------------------------------------
# Tests (ctest)
# ---------------------------------------------------------------------------
include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
