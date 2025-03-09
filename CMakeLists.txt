cmake_minimum_required(VERSION 3.21)

project(dmtimer)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(cmake/ModuleImport.cmake)
include(cmake/ModuleCompileOptions.cmake)
include(cmake/ModuleCompilerDefenses.cmake)

ModuleSetCompileOptions()

option(DMTIMER_USE_HIGH_RESOLUTION "use high resolution" OFF)
option(USE_CLANG_TIDY "enable clang-tidy" OFF)

if(WIN32)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
    add_definitions(/wd4996)
else()
    if(USE_CLANG_TIDY AND NOT WIN32)
        find_program(CLANG_TIDY_EXE NAMES clang-tidy)
        if(CLANG_TIDY_EXE)
            set(CMAKE_CXX_CLANG_TIDY 
                "${CLANG_TIDY_EXE};--config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy"
                CACHE STRING "clang-tidy command" FORCE
            )
        endif()
    endif()
endif(WIN32)

if(DMTIMER_USE_HIGH_RESOLUTION)
    add_definitions(-DDMTIMER_USE_HIGH_RESOLUTION)
endif()

LibImport("dmtimer" "src")

if (PROJECT_IS_TOP_LEVEL) # need cmake 3.21+
    ExeImport("test" "dmtimer")
endif(PROJECT_IS_TOP_LEVEL)
