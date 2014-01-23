# CMake build control file for Serialization Library tests

cmake_minimum_required(VERSION 2.8)

project("Serialization Library Test")

#
# Compiler settings
#

message(STATUS "compiler is ${CMAKE_CXX_COMPILER_ID}" )

if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
  add_definitions( -ftemplate-depth=300 )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
  add_definitions( /wd4996 )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
  add_definitions( -ftemplate-depth=300 )
  set (CMAKE_CXX_FLAGS "-std=c++11" )
  set (CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O3" )
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -dead_strip")
endif()

if( CMAKE_HOST_APPLE )
  set (CMAKE_OSX_ARCHITECTURES "x86_64" )
endif()

#
# Locate Project Prerequisites 
#

# Boost

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS serialization wserialization system filesystem)

if(Boost_FOUND)
    message(STATUS "Boost is ${BOOST_ROOT}")
    message(STATUS "Boost directories found at ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries found at ${Boost_LIBRARY_DIRS}")
    message(STATUS "Boost component libraries to be linked are ${Boost_LIBRARIES}")
    message(STATUS "Boost version found is ${Boost_VERSION}")
    include(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
elseif()
    message("Boost NOT Found!")
endif()

file( GLOB test_files ../test/test*.cpp)

foreach( test_file ${test_files} )
  string(REGEX REPLACE ".*(test_.*).cpp" "\\1" test_name ${test_file} )

  message(STATUS "${test_name}" )

  add_executable( ${test_name} ../test/${test_name}.cpp )
  target_link_libraries(${test_name} ${Boost_LIBRARIES})

endforeach()

