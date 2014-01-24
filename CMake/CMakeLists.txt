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
    message(STATUS "Address model is 64 bits")
    set (CMAKE_OSX_ARCHITECTURES "x86_64" )
  elseif(Boost_ADDRESS_MODEL EQUAL 32)
    message(STATUS "Address model is 32 bits")
    set (CMAKE_OSX_ARCHITECTURES "i386" )
  else()
    message(FATAL_ERROR "Address model must be either 32 or 64")
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
    include_directories("${Boost_INCLUDE_DIRS}")
    link_directories("${Boost_LIBRARY_DIRS}")
elseif()
    message("Boost NOT Found!")
endif()

function( serialization_test test_name)
  set(arglist)
  foreach(a IN ITEMS ${ARGN} )
    set(arglist ${arglist} ../test/${a}.cpp)
  endforeach()
  message(STATUS ${test_name})
  add_executable( ${test_name} ../test/${test_name}.cpp ${arglist} )
  target_link_libraries(${test_name} ${Boost_LIBRARIES})
  add_test( ${test_name} ${test_name} )
endfunction(serialization_test)

function(archive_test test_name)
  set(arglist)
  foreach(a IN ITEMS ${ARGN} )
    set(arglist ${arglist} ../test/${a}.cpp)
  endforeach()
  foreach(
    archive-name
    IN ITEMS text_archive text_warchive binary_archive xml_archive xml_warchive
  )
    set(amended_test_name ${test_name}_${archive-name})
    message(STATUS ${amended_test_name})
    set_property(
      SOURCE ../test/${test_name}.cpp 
      PROPERTY COMPILE_DEFINITIONS BOOST_ARCHIVE_TEST=${archive-name}.hpp
    )
    add_executable(${amended_test_name} ../test/${test_name}.cpp ${arglist})
    target_link_libraries(${amended_test_name} ${Boost_LIBRARIES})
    add_test(${amended_test_name} ${amended_test_name})
  endforeach()
endfunction(archive_test)

function(polymorphic_archive_test test_name)
  set(arglist)
  foreach(a IN ITEMS ${ARGN} )
    set(arglist ${arglist} ../test/${a}.cpp)
  endforeach()
  foreach(
    archive-name
    IN ITEMS text_archive text_warchive binary_archive xml_archive xml_warchive
  )
    set(amended_test_name ${test_name}_polymorphic_${archive-name})
    message(STATUS ${amended_test_name})
    set_property(
      SOURCE ../test/${test_name}.cpp 
      PROPERTY COMPILE_DEFINITIONS BOOST_ARCHIVE_TEST=${archive-name}.hpp
    )
    add_executable(${amended_test_name} ../test/${test_name}.cpp ${arglist})
    target_link_libraries(${amended_test_name} ${Boost_LIBRARIES})
    add_test(${amended_test_name} ${amended_test_name})
  endforeach()
endfunction(polymorphic_archive_test)

enable_testing()

# serialization(test_dll_exported dll_polymorphic_derived2_lib)
# serialization(test_dll_simple dll_a_lib)
# compile test_dll_plugin.cpp
# Running the following test requires that the test know the directory 
# in which the dll is stored. I don't know how to extract this from bjam
# serialization(test_dll_plugin : : dll_polymorphic_derived2_lib)

serialization_test(test_private_ctor)
serialization_test(test_reset_object_address A)
serialization_test(test_void_cast)
serialization_test(test_mult_archive_types)
serialization_test(test_iterators)
serialization_test(test_iterators_base64)
serialization_test(test_inclusion)
serialization_test(test_utf8_codecvt ../src/utf8_codecvt_facet)
serialization_test(test_codecvt_null ../src/codecvt_null)

archive_test(test_array A)
archive_test(test_binary)
archive_test(test_bitset)
archive_test(test_complex)
archive_test(test_contained_class A)
archive_test(test_cyclic_ptrs A)
archive_test(test_delete_pointer)
archive_test(test_deque A)
archive_test(test_derived)
archive_test(test_derived_class A)
archive_test(test_diamond)
archive_test(test_diamond_complex)
archive_test(test_exported polymorphic_base)
archive_test(test_class_info_load)
archive_test(test_class_info_save)
archive_test(test_object)
archive_test(test_primitive)
archive_test(test_list A)
archive_test(test_list_ptrs A)
archive_test(test_map A)
archive_test(test_mi)
archive_test(test_multiple_ptrs A)
archive_test(test_multiple_inheritance)
archive_test(test_no_rtti polymorphic_base polymorphic_derived1)
archive_test(test_non_intrusive)
archive_test(test_non_default_ctor)
archive_test(test_non_default_ctor2)
archive_test(test_null_ptr)
archive_test(test_nvp A)
archive_test(test_recursion A)
archive_test(test_registered)
archive_test(test_set)
archive_test(test_simple_class A)
archive_test(test_simple_class_ptr A)
archive_test(test_split)
archive_test(test_tracking)
archive_test(test_unregistered)
archive_test(test_valarray)
archive_test(test_variant A)
archive_test(test_vector A)
archive_test(test_new_operator A)
archive_test(test_optional)
archive_test(test_shared_ptr)
archive_test(test_shared_ptr_multi_base)
archive_test(test_shared_ptr_132)

polymorphic_archive_test(test_polymorphic test_polymorphic_A A )
polymorphic_archive_test(test_polymorphic2 test_polymorphic2imp)

