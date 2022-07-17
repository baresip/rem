#
# CMakeLists.txt
#
# Copyright (C) 2010 - 2022 Alfred E. Heggestad
#

##############################################################################
#
# Versioning
#

cmake_minimum_required(VERSION 3.7)

project(rem VERSION 2.6.0 LANGUAGES C)

set(PROJECT_SOVERSION 3) # bump if ABI breaks

##############################################################################
#
# Module Includes
#
include(CheckIncludeFile)

##############################################################################
#
# Compile options
#

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)

if(NOT MSVC)
  add_compile_options(
    -Waggregate-return
    -Wall
    -Wbad-function-cast
    -Wcast-align
    -Wdeclaration-after-statement
    -Wextra
    -Wmissing-declarations
    -Wmissing-prototypes
    -Wnested-externs
    -Wold-style-definition
    -Wshadow
    -Wstrict-prototypes
    -Wvla
  )
endif()

check_include_file(unistd.h HAVE_UNISTD_H)
if(HAVE_UNISTD_H)
  add_definitions(-DHAVE_UNISTD_H)
endif()

include_directories(
  include
  ../re/include
  /usr/local/include
  /usr/local/include/re
)
link_directories(/usr/local/lib)
add_definitions(-DVERSION="${PROJECT_VERSION}")

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

add_library(${PROJECT_NAME}
  ${SRCS}
  ${HEADERS}
)

##############################################################################
#
# Main target object
#

target_link_libraries(${PROJECT_NAME} re -lpthread -lm)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_SOVERSION})

set_target_properties(rem PROPERTIES PUBLIC_HEADER "${HEADERS}")
install(TARGETS rem LIBRARY)
