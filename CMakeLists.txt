set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 2.6)
project(odatacpp-server)

enable_testing()

set(WARNINGS)

# Platform (not compiler) specific settings
if(UNIX) # This includes OSX
  find_package(Boost REQUIRED COMPONENTS locale filesystem system)
  find_package(LibXml2 REQUIRED)

  option(BUILD_SHARED_LIBS "Build shared Libraries." ON)
else()
  message(FATAL_ERROR "-- Unsupported Build Platform.")
endif()

# Compiler (not platform) specific settings
if(("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
  message("-- Setting clang options")

  set(WARNINGS "-Wall -Wextra -Wcast-qual -Wformat=2 -Winit-self -Winvalid-pch -Wmissing-format-attribute -Wmissing-include-dirs -Wpacked -Wredundant-decls")
  set(OSX_SUPPRESSIONS "-Wno-overloaded-virtual -Wno-sign-conversion -Wno-deprecated -Wno-unknown-pragmas -Wno-reorder -Wno-char-subscripts -Wno-switch -Wno-unused-parameter -Wno-unused-variable -Wno-deprecated -Wno-unused-value -Wno-unknown-warning-option -Wno-return-type-c-linkage -Wno-unused-function -Wno-sign-compare -Wno-shorten-64-to-32 -Wno-reorder -Wno-ignored-qualifiers -Wno-sometimes-uninitialized -Wno-logical-op-parentheses -Wno-unused-private-field")
  set(WARNINGS "${WARNINGS} ${OSX_SUPPRESSIONS}")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wno-return-type-c-linkage -Wno-unneeded-internal-declaration")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-strict-aliasing")
  set(STRICT_CXX_FLAGS ${WARNINGS} "-Werror -pedantic")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  message("-- Setting gcc options")

  set(WARNINGS "-Wall -Wextra -Wcast-align -Wformat=2 -Winit-self -Winvalid-pch -Wmissing-format-attribute -Wmissing-include-dirs -Wpacked -Wredundant-decls -Wunreachable-code")
  set(LINUX_SUPPRESSIONS "-Wno-deprecated -Wno-unknown-pragmas -Wno-reorder -Wno-unused-function -Wno-char-subscripts -Wno-switch -Wno-unused-but-set-parameter -Wno-unused-value -Wno-unused-local-typedefs -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wno-ignored-qualifiers -Wno-cast-qual -Wno-parentheses -Wno-return-type")

  set(WARNINGS "${WARNINGS} ${LINUX_SUPPRESSIONS}")
  set(LD_FLAGS "${LD_FLAGS} -Wl,-z,defs")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-strict-aliasing")
  set(STRICT_CXX_FLAGS ${WARNINGS} "-Werror -pedantic")
else()
  message("-- Unknown compiler, success is doubtful.")
endif()

# Reconfigure final output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output)

# These settings can be used by the test targets
set(ODATACPP_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(ODATACPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ODATACPP_INCLUDE_DIRS ${ODATACPP_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})
set(ODATACPP_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# OData library targets
set(ODATACPP_LIBRARY ${LIB}odata-library)

# Everything in the project needs access to the odatacpp include directories
include_directories(${ODATACPP_INCLUDE_DIRS})

add_subdirectory(src)
add_subdirectory(tests)
