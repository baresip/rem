#
# CMakeLists.txt
#
# Copyright (C) 2010 - 2022 Alfred E. Heggestad
#

##############################################################################
#
# Versioning
#

cmake_minimum_required(VERSION 3.10)

project(rem
  VERSION 2.9.0
  LANGUAGES C
  HOMEPAGE_URL https://github.com/baresip/rem
  DESCRIPTION "Audio and video processing media library"
)

set(PROJECT_SOVERSION 3) # bump if ABI breaks

# Pre-release identifier, comment out on a release
# Increment for breaking changes (dev2, dev3...)
#set(PROJECT_VERSION_PRE dev)

if(PROJECT_VERSION_PRE)
  set(PROJECT_VERSION_FULL ${PROJECT_VERSION}-${PROJECT_VERSION_PRE})
else()
  set(PROJECT_VERSION_FULL ${PROJECT_VERSION})
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

##############################################################################
#
# Module Includes
#
include(GNUInstallDirs)
include(CheckIncludeFile)

find_package(RE REQUIRED)

##############################################################################
#
# Compile options
#

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)

if(MSVC)
  add_compile_options("/W3")
else()
  add_compile_options(
    -pedantic
    -Wall
    -Wbad-function-cast
    -Wcast-align
    -Wextra
    -Wmissing-declarations
    -Wmissing-prototypes
    -Wnested-externs
    -Wno-strict-aliasing
    -Wold-style-definition
    -Wshadow -Waggregate-return
    -Wstrict-prototypes
    -Wuninitialized
    -Wvla
  )
endif()

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wshorten-64-to-32 -Watomic-implicit-seq-cst)
endif()

find_package(re CONFIG REQUIRED HINTS ../re/cmake)

list(APPEND RE_DEFINITIONS
  -DVERSION="${PROJECT_VERSION_FULL}"
  -DVER_MAJOR=${PROJECT_VERSION_MAJOR}
  -DVER_MINOR=${PROJECT_VERSION_MINOR}
  -DVER_PATCH=${PROJECT_VERSION_PATCH}
)

##############################################################################
#
# Source/Header section
#

set(SRCS
  src/aac/aac.c
  src/au/fmt.c
  src/au/util.c
  src/aubuf/aubuf.c
  src/aubuf/ajb.c
  src/auconv/auconv.c
  src/aufile/aufile.c
  src/aufile/wave.c
  src/auframe/auframe.c
  src/aulevel/aulevel.c
  src/aumix/aumix.c
  src/auresamp/resamp.c
  src/autone/tone.c
  src/avc/config.c
  src/dtmf/dec.c
  src/fir/fir.c
  src/g711/g711.c
  src/goertzel/goertzel.c
  src/vid/draw.c
  src/vid/fmt.c
  src/vid/frame.c
  src/vidconv/vconv.c
  src/vidmix/vidmix.c
)

set(HEADERS
  include/rem_aac.h
  include/rem_aubuf.h
  include/rem_auconv.h
  include/rem_audio.h
  include/rem_aufile.h
  include/rem_auframe.h
  include/rem_au.h
  include/rem_aulevel.h
  include/rem_aumix.h
  include/rem_auresamp.h
  include/rem_autone.h
  include/rem_avc.h
  include/rem_dsp.h
  include/rem_dtmf.h
  include/rem_fir.h
  include/rem_flv.h
  include/rem_g711.h
  include/rem_goertzel.h
  include/rem.h
  include/rem_vidconv.h
  include/rem_video.h
  include/rem_vid.h
  include/rem_vidmix.h
)

##############################################################################
#
# Linking LIBS
#

set(LINKLIBS ${RE_LIBRARIES} Threads::Threads)

if(WIN32)
  list(APPEND LINKLIBS qwave iphlpapi wsock32 ws2_32)
else()
  list(APPEND LINKLIBS -lm)
endif()


##############################################################################
#
# Main target object
#

add_library(rem-objs OBJECT ${SRCS} ${HEADERS})

set_target_properties(rem-objs PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(rem-objs PRIVATE ${RE_DEFINITIONS})

target_link_libraries(rem-objs ${LINKLIBS})
target_include_directories(rem-objs
  PUBLIC include 
  PRIVATE ${RE_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR}
)


##############################################################################
#
# Shared target librem.[so|dll|dylib]
#

add_library(rem-shared SHARED $<TARGET_OBJECTS:rem-objs>)
target_link_libraries(rem-shared PRIVATE ${LINKLIBS})
set_target_properties(rem-shared PROPERTIES VERSION
  ${PROJECT_SOVERSION}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set_target_properties(rem-shared PROPERTIES SOVERSION ${PROJECT_SOVERSION})
set_target_properties(rem-shared PROPERTIES OUTPUT_NAME "rem")


##############################################################################
#
# Static target librem.a
#

add_library(rem STATIC $<TARGET_OBJECTS:rem-objs>)
target_link_libraries(rem PUBLIC ${LINKLIBS})
target_include_directories(rem PUBLIC include)
set_target_properties(rem PROPERTIES PUBLIC_HEADER "${HEADERS}")

if(MSVC)
set_target_properties(rem PROPERTIES OUTPUT_NAME "rem-static")
endif()


##############################################################################
#
# Packaging section
#

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  add_subdirectory(packaging)
endif()

configure_file(packaging/librem.pc.in librem.pc @ONLY)


##############################################################################
#
# Install section
#

install(TARGETS rem-shared rem
  LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT Libraries
    NAMELINK_SKIP
  ARCHIVE
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT Development
  PUBLIC_HEADER
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rem
    COMPONENT Development
)

install(TARGETS rem-shared
  LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    NAMELINK_ONLY
    COMPONENT Development
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/librem.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  COMPONENT Development
)
