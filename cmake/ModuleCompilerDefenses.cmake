# 通用CMake文件，用于在不同编译器上启用堆栈保护标志
# 该脚本设置堆栈保护以增强安全性

# 初始化链接器防御标志变量
set(PUBLIC_LINKER_DEFENSES_FLAGS_COMMON "")

MACRO(public_check_compiler_flag LANG FLAG RESULT)
  set(_fname "${ARGN}")
  if(NOT DEFINED ${RESULT})
    if(_fname)
      # nothing
    elseif("_${LANG}_" MATCHES "_CXX_")
      set(_fname "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.cxx")
      if("${CMAKE_CXX_FLAGS} ${FLAG} " MATCHES "-Werror " OR "${CMAKE_CXX_FLAGS} ${FLAG} " MATCHES "-Werror=unknown-pragmas ")
        FILE(WRITE "${_fname}" "int main() { return 0; }\n")
      else()
        FILE(WRITE "${_fname}" "#pragma\nint main() { return 0; }\n")
      endif()
    elseif("_${LANG}_" MATCHES "_C_")
      set(_fname "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.c")
      if("${CMAKE_C_FLAGS} ${FLAG} " MATCHES "-Werror " OR "${CMAKE_C_FLAGS} ${FLAG} " MATCHES "-Werror=unknown-pragmas ")
        FILE(WRITE "${_fname}" "int main(void) { return 0; }\n")
      else()
        FILE(WRITE "${_fname}" "#pragma\nint main(void) { return 0; }\n")
      endif()
    elseif("_${LANG}_" MATCHES "_OBJCXX_")
      set(_fname "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.mm")
      if("${CMAKE_CXX_FLAGS} ${FLAG} " MATCHES "-Werror " OR "${CMAKE_CXX_FLAGS} ${FLAG} " MATCHES "-Werror=unknown-pragmas ")
        FILE(WRITE "${_fname}" "int main() { return 0; }\n")
      else()
        FILE(WRITE "${_fname}" "#pragma\nint main() { return 0; }\n")
      endif()
    else()
      unset(_fname)
    endif()
    if(_fname)
      if(NOT "x${ARGN}" STREQUAL "x")
        file(RELATIVE_PATH __msg "${CMAKE_SOURCE_DIR}" "${ARGN}")
        set(__msg " (check file: ${__msg})")
      else()
        set(__msg "")
      endif()
      if(CMAKE_REQUIRED_LIBRARIES)
        set(__link_libs LINK_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
      else()
        set(__link_libs)
      endif()
      set(__cmake_flags "")
      if(CMAKE_EXE_LINKER_FLAGS)  # CMP0056 do this on new CMake
        list(APPEND __cmake_flags "-DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}")
      endif()

      # CMP0067 do this on new CMake
      if(DEFINED CMAKE_CXX_STANDARD)
        list(APPEND __cmake_flags "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}")
      endif()
      if(DEFINED CMAKE_CXX_STANDARD_REQUIRED)
        list(APPEND __cmake_flags "-DCMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED}")
      endif()
      if(DEFINED CMAKE_CXX_EXTENSIONS)
        list(APPEND __cmake_flags "-DCMAKE_CXX_EXTENSIONS=${CMAKE_CXX_EXTENSIONS}")
      endif()

      MESSAGE(STATUS "Performing Test ${RESULT}${__msg}")
      TRY_COMPILE(${RESULT}
        "${CMAKE_BINARY_DIR}"
        "${_fname}"
        CMAKE_FLAGS ${__cmake_flags}
        COMPILE_DEFINITIONS "${FLAG}"
        ${__link_libs}
        OUTPUT_VARIABLE OUTPUT)

      IF(${RESULT})
        SET(${RESULT} 1 CACHE INTERNAL "Test ${RESULT}")
        MESSAGE(STATUS "Performing Test ${RESULT} - Success")
      ELSE(${RESULT})
        MESSAGE(STATUS "Performing Test ${RESULT} - Failed")
        SET(${RESULT} "" CACHE INTERNAL "Test ${RESULT}")
        file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
            "Compilation failed:\n"
            "    source file: '${_fname}'\n"
            "    check option: '${FLAG}'\n"
            "===== BUILD LOG =====\n"
            "${OUTPUT}\n"
            "===== END =====\n\n")
      ENDIF(${RESULT})
    else()
      SET(${RESULT} 0)
    endif()
  endif()
ENDMACRO()

macro(public_check_flag_support lang flag varname base_options)
  if(CMAKE_BUILD_TYPE)
    set(CMAKE_TRY_COMPILE_CONFIGURATION ${CMAKE_BUILD_TYPE})
  endif()

  if("_${lang}_" MATCHES "_CXX_")
    set(_lang CXX)
  elseif("_${lang}_" MATCHES "_C_")
    set(_lang C)
  elseif("_${lang}_" MATCHES "_OBJCXX_")
    if(DEFINED CMAKE_OBJCXX_COMPILER)  # CMake 3.16+ and enable_language(OBJCXX) call are required
      set(_lang OBJCXX)
    else()
      set(_lang CXX)
    endif()
  else()
    set(_lang ${lang})
  endif()

  string(TOUPPER "${flag}" ${varname})
  string(REGEX REPLACE "^(/|-)" "HAVE_${_lang}_" ${varname} "${${varname}}")
  string(REGEX REPLACE " -|-|=| |\\.|," "_" ${varname} "${${varname}}")

  if(DEFINED CMAKE_${_lang}_COMPILER)
    public_check_compiler_flag("${_lang}" "${base_options} ${flag}" ${${varname}} ${ARGN})
  endif()
endmacro()

# 定义宏函数以添加支持的编译器标志
macro(public_add_defense_compiler_flag option)
  # 检查C++编译器是否支持该选项
  public_check_flag_support(CXX "${option}" _varname "${ARGN}")
  if(${_varname})
    # 如果支持，将该选项添加到C++编译器标志中
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${option}")
  endif()

  # 检查C编译器是否支持该选项
  public_check_flag_support(C "${option}" _varname "${ARGN}")
  if(${_varname})
    # 如果支持，将该选项添加到C编译器标志中
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${option}")
  endif()
endmacro()

# 定义宏函数以在发布版本中添加支持的编译器标志
macro(public_add_defense_compiler_flag_release option)
  # 检查C++编译器是否支持该选项
  public_check_flag_support(CXX "${option}" _varname "${ARGN}")
  if(${_varname})
    # 如果支持，将该选项添加到发布版本的C++编译器标志中
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${option}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${option}")
  endif()

  # 检查C编译器是否支持该选项
  public_check_flag_support(C "${option}" _varname "${ARGN}")
  if(${_varname})
    # 如果支持，将该选项添加到发布版本的C编译器标志中
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${option}")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${option}")    
  endif()
endmacro()

# 为不同的编译器定义堆栈保护标志
if(MSVC)
  # 为MSVC启用缓冲区安全检查
  public_add_defense_compiler_flag("/GS")
  # 启用附加安全检查
  public_add_defense_compiler_flag("/sdl")
  # 启用控制流保护
  public_add_defense_compiler_flag("/guard:cf")
  # 启用一系列的警告标志
  public_add_defense_compiler_flag("/w34018 /w34146 /w34244 /w34267 /w34302 /w34308 /w34509 /w34532 /w34533 /w34700 /w34789 /w34995 /w34996")
  # 设置链接器防御标志
  set(PUBLIC_LINKER_DEFENSES_FLAGS_COMMON "${PUBLIC_LINKER_DEFENSES_FLAGS_COMMON} /guard:cf /dynamicbase")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  # 为Clang启用强堆栈保护
  public_add_defense_compiler_flag("-fstack-protector-strong")
  # 为发布版本启用_FORTIFY_SOURCE
  public_add_defense_compiler_flag_release("-D_FORTIFY_SOURCE=2")
  if (NOT APPLE)
    # 为非Apple平台设置链接器防御标志
    set(PUBLIC_LINKER_DEFENSES_FLAGS_COMMON "${PUBLIC_LINKER_DEFENSES_FLAGS_COMMON} -z noexecstack -z relro -z now")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "GNU")
  # 为旧版本的GCC启用堆栈保护
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
    public_add_defense_compiler_flag("-fstack-protector")
  else()
    # 为新版GCC启用强堆栈保护
    public_add_defense_compiler_flag("-fstack-protector-strong")
  endif()

  # 设置GCC的链接器防御标志
  set(PUBLIC_LINKER_DEFENSES_FLAGS_COMMON "${PUBLIC_LINKER_DEFENSES_FLAGS_COMMON} -z noexecstack -z relro -z now")
else()
  # 其他编译器不支持堆栈保护标志
  message(WARNING "Stack protection flags are not supported for this compiler.")
endif()

# 确保使用位置无关代码
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
if(CMAKE_CXX_COMPILER MATCHES "gcc" OR CMAKE_CXX_COMPILER MATCHES "clang")
  if(NOT CMAKE_CXX_FLAGS MATCHES "-fPIC")
    # 启用位置无关代码
    public_add_defense_compiler_flag("-fPIC")
  endif()
  # 设置可执行文件的链接器标志
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")
endif()

# 设置共享库、模块和可执行文件的链接器标志
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${PUBLIC_LINKER_DEFENSES_FLAGS_COMMON}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${PUBLIC_LINKER_DEFENSES_FLAGS_COMMON}")
set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${PUBLIC_LINKER_DEFENSES_FLAGS_COMMON}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PUBLIC_LINKER_DEFENSES_FLAGS_COMMON}")
