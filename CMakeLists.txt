cmake_minimum_required(VERSION 3.20)

project(
    DispatchIQ
    VERSION 0.1.0
    DESCRIPTION "Emergency response dispatch simulator"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(dispatchiq
    src/main.cpp
    include/dispatchiq/incident.hpp
    src/incident.cpp
    include/dispatchiq/emergency_call.hpp
    src/emergency_call.cpp
    include/dispatchiq/incident_triage.hpp
    src/incident_triage.cpp
)

target_compile_features(dispatchiq PRIVATE cxx_std_20)
set_target_properties(dispatchiq PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(dispatchiq PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)

if(MSVC)
    target_compile_options(dispatchiq PRIVATE /W4)
else()
    target_compile_options(dispatchiq PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
