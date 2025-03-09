cmake_minimum_required(VERSION 3.21)

project(dmtimer)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(cmake/ModuleImport.cmake)
include(cmake/ModuleCompileOptions.cmake)
include(cmake/ModuleCompilerDefenses.cmake)

ModuleSetCompileOptions()

option(DMTIMER_USE_HIGH_RESOLUTION "use high resolution" on)

if(WIN32)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
    add_definitions(/wd4996)
else()
    set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)
endif(WIN32)

if(DMTIMER_USE_HIGH_RESOLUTION)
    add_definitions(-DDMTIMER_USE_HIGH_RESOLUTION)
endif()

LibImport("dmtimer" "src")

if (PROJECT_IS_TOP_LEVEL) # need cmake 3.21+
    ExeImport("test" "dmtimer")
endif(PROJECT_IS_TOP_LEVEL)
