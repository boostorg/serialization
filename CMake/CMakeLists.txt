# CMake build control file for Serialization Library tests

cmake_minimum_required(VERSION 2.8)

project("SerializationLibraryTest")

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

#
# IDE settings
#

if( CMAKE_HOST_APPLE )
  if(Boost_ADDRESS_MODEL EQUAL 64)
    message("Address model is 64 bits")
    set (CMAKE_OSX_ARCHITECTURES "x86_64" )
  elseif(Boost_ADDRESS_MODEL EQUAL 32)
    message("Address model is 32 bits")
    set (CMAKE_OSX_ARCHITECTURES "i386" )
  else()
    message("Address model must be either 32 or 64")
  endif()
endif()

#
# Locate Project Prerequisites 
#

# Boost

# note: we're assuming that boost has been built with:
# ./b2 —-layout=versioned toolset=clang-darwin link=static address-model=64 stage

set(Boost_ADDRESS_MODEL 64 CACHE INTEGER "32/64 bits")
set(Boost_USE_STATIC_LIBS ON CACHE BOOL "Link to Boost static libraries")
set(Boost_USE_MULTITHREADED ON)
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

