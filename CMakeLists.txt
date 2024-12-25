cmake_minimum_required(VERSION 3.21)

PROJECT(dmtimer)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
INCLUDE(cmake/ModuleCompilerDefenses.cmake)

ModuleSetCompileOptions()

OPTION(DMTIMER_USE_HIGH_RESOLUTION "use high resolution" OFF)

if(WIN32)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
    add_definitions(/wd4996)
endif(WIN32)

if(DMTIMER_USE_HIGH_RESOLUTION)
    add_definitions(-DDMTIMER_USE_HIGH_RESOLUTION)
endif()

LibImport("dmtimer" "src")

if (PROJECT_IS_TOP_LEVEL) # need cmake 3.21+
    ExeImport("test" "dmtimer")
endif(PROJECT_IS_TOP_LEVEL)
