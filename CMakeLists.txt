cmake_minimum_required(VERSION 2.6)
project(wtclasses)

enable_testing()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 3)
set(VERSION_PATCH 5.5)
set(SONAME 11)

set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(local_include_dir ${PROJECT_BINARY_DIR}/include)
set(local_headers_dir ${local_include_dir}/Wt/Wc)
set(local_headers ${local_headers_dir}/global.hpp)

set(datadir ${CMAKE_INSTALL_PREFIX}/share CACHE PATH "Root of data files")
set(WT_DIR ${datadir}/Wt CACHE PATH "Wt installation dir")
set(DOC_SUBDIR ${datadir}/doc/libwtclasses CACHE PATH "Docs subdir")

include(${CMAKE_CURRENT_SOURCE_DIR}/features/Features.cmake)

subdirs(src locales css js)

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

