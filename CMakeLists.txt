cmake_minimum_required(VERSION 3.21)

PROJECT(dmtimer)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
INCLUDE(cmake/ModuleCompilerDefenses.cmake)

ModuleSetCompileOptions()

if(WIN32)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
    add_definitions(/wd4996)
endif(WIN32)

LibImport("dmtimer" "src")

if (PROJECT_IS_TOP_LEVEL) # need cmake 3.21+
    ExeImport("test" "dmtimer")
endif(PROJECT_IS_TOP_LEVEL)
