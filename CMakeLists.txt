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

project(rem VERSION 2.7.0 LANGUAGES C)

set(PROJECT_SOVERSION 3) # bump if ABI breaks

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
# Main target object
#

add_library(${PROJECT_NAME} ${SRCS} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${RE_LIBRARIES} Threads::Threads -lm)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${RE_DEFINITIONS})
target_include_directories(${PROJECT_NAME}
  PUBLIC include 
  PRIVATE ${RE_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR}
)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_SOVERSION})

set_target_properties(rem PROPERTIES PUBLIC_HEADER "${HEADERS}")


##############################################################################
#
# Install section
#

install(TARGETS rem
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

install(TARGETS rem
  LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    NAMELINK_ONLY
    COMPONENT Development
)
