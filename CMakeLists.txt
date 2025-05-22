cmake_minimum_required(VERSION 3.21)

project(dmtimer)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(cmake/ModuleImport.cmake)
include(cmake/ModuleCompileOptions.cmake)
include(cmake/ModuleCompilerDefenses.cmake)

ModuleSetCompileOptions()

set(DMTIMER_VERSION_MAJOR "1")
set(DMTIMER_VERSION_MINOR "3")
set(DMTIMER_VERSION_PATCH "2")
set(DMTIMER_VERSION "${DMTIMER_VERSION_MAJOR}.${DMTIMER_VERSION_MINOR}.${DMTIMER_VERSION_PATCH}")
mark_as_advanced(DMTIMER_VERSION)

option(DMTIMER_USE_HIGH_RESOLUTION "use high resolution" OFF)
option(USE_CLANG_TIDY "enable clang-tidy" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

if(WIN32)
    if(MSVC)
        add_compile_options(/Bt)
    endif()

    set(CMAKE_DEPENDS_USE_COMPILER TRUE)
else()
    if(USE_CLANG_TIDY)
        set(CMAKE_CXX_CLANG_TIDY clang-tidy
        --config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy
        )
    endif()
endif(WIN32)

if(DMTIMER_USE_HIGH_RESOLUTION)
    add_definitions(-DDMTIMER_USE_HIGH_RESOLUTION)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/dmtimer_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/dmtimer_config.h)

LibImport("dmtimer" "src")

if (PROJECT_IS_TOP_LEVEL) # need cmake 3.21+
    ExeImportAndTest("test" "dmtimer")
endif()

AddInstall("dmtimer" "${CMAKE_CURRENT_SOURCE_DIR}/include")