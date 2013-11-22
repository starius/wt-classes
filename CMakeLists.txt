cmake_minimum_required(VERSION 2.6)
project(wtclasses)

enable_testing()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 4)
set(VERSION_PATCH 0)
set(SONAME 12)

set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(local_include_dir ${PROJECT_BINARY_DIR}/include)
set(local_headers_dir ${local_include_dir}/Wt/Wc)

set(CMAKE_INSTALL_PREFIX /usr/local CACHE PATH "installation dir for wt-classes")
set(datadir ${CMAKE_INSTALL_PREFIX}/share CACHE PATH "Root of data files")
set(WT_DIR ${datadir}/Wt CACHE PATH "Wt installation dir")
set(BOOST_DIR ${datadir}/Boost CACHE PATH "Boost installation dir")
INCLUDE_DIRECTORIES(
${WT_DIR}/include
${BOOST_DIR}/include
)
LINK_DIRECTORIES(
${WT_DIR}/lib
${BOOST_DIR}/lib
)
set (WT_LIB_SUFFIX "d" CACHE STRING "Wt lib suffix")
set (BOOST_LIB_SUFFIX "-mt" CACHE STRING "Wt lib suffix" )
set(DOC_SUBDIR ${datadir}/doc/libwtclasses CACHE PATH "Docs subdir")

include(${CMAKE_CURRENT_SOURCE_DIR}/features/ubuntu-gcc.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/features/Features.cmake)

subdirs(src)
file(GLOB local_headers "${local_headers_dir}/*.hpp")

subdirs(locales css)

option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    subdirs(examples)
endif()

option(BUILD_DOC "Build documentation" ON)

if(BUILD_DOC)
    subdirs(doc)
endif()

option(BUILD_LOCALES_TEST "Build locales-test" ON)
if(BUILD_LOCALES_TEST)
    subdirs(locales-test)
endif()

