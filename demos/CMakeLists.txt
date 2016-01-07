# =========================================================================
# Freetype GL - A C OpenGL Freetype engine
# Platform:    Any
# WWW:         https://github.com/rougier/freetype-gl
# -------------------------------------------------------------------------
# Copyright 2011,2012 Nicolas P. Rougier. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  1. Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are
# those of the authors and should not be interpreted as representing official
# policies, either expressed or implied, of Nicolas P. Rougier.
# =========================================================================
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.5 )

MACRO( DEMO _target _sources)
     add_executable(${_target} ${_sources})
    if(NOT APPLE)
     target_link_libraries(${_target} ${GLEW_LIBRARY})
    endif()
     target_link_libraries(${_target} freetype-gl)
     target_link_libraries(${_target} ${OPENGL_LIBRARY})
     target_link_libraries(${_target} ${GLFW3_LIBRARY})
     target_link_libraries(${_target} ${FREETYPE_LIBRARIES})
     IF( MATH_LIBRARY )
         target_link_libraries(${_target} ${MATH_LIBRARY})
     ENDIF( MATH_LIBRARY )
    if(MSVC)
      # Compile demos as C++
      set_source_files_properties(${_sources} PROPERTIES LANGUAGE CXX)
    endif()
ENDMACRO( DEMO )

IF( WIN32 OR WIN64 )
    SET( ENV{ANT_TWEAK_BAR_ROOT}
         ${CMAKE_CURRENT_SOURCE_DIR}/../windows/AntTweakBar )
ENDIF( WIN32 OR WIN64 )

# Find dependencies
FIND_PACKAGE( glfw3 REQUIRED )
FIND_PACKAGE( AntTweakBar )
FIND_PACKAGE( FontConfig )

INCLUDE_DIRECTORIES(
    ${GLFW3_INCLUDE_DIRS}
)

include_directories(
    ${GLFW3_INCLUDE_DIR}
    ${ANT_TWEAK_BAR_INCLUDE_PATH}
    ${FONTCONFIG_INCLUDE_DIR}
)

LINK_DIRECTORIES(
    ${GLFW3_LIBRARY_DIR}
)

ADD_CUSTOM_COMMAND(
    DEPENDS
        makefont
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    COMMAND
        ../makefont
    ARGS
        --font fonts/Vera.ttf
        --header ${CMAKE_CURRENT_SOURCE_DIR}/vera-16.h
        --size 16
        --variable font
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/vera-16.h)
ADD_CUSTOM_TARGET(generate-embedded-font
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/vera-16.h)
DEMO( embedded-font    "embedded-font.c" )
ADD_DEPENDENCIES(embedded-font generate-embedded-font)

DEMO( texture          "texture.c" )
DEMO( font             "font.c" )
DEMO( benchmark        "benchmark.c" )
DEMO( console          "console.c" )
DEMO( cube             "cube.c" )
DEMO( glyph            "glyph.c" )
DEMO( cartoon          "cartoon.c" )
DEMO( outline          "outline.c" )
DEMO( lcd              "lcd.c" )
DEMO( gamma            "gamma.c" )
DEMO( ansi             "ansi.c" )
DEMO( subpixel         "subpixel.c" )
DEMO( distance-field   "distance-field.c")
DEMO( distance-field-2 "distance-field-2.c")
DEMO( distance-field-3 "distance-field-3.c")

IF (FONTCONFIG_FOUND)
   DEMO( markup      "markup.c" )
   target_link_libraries(markup ${FONTCONFIG_LIBRARY})
ENDIF (FONTCONFIG_FOUND)

IF (ANT_TWEAK_BAR_FOUND AND (NOT (WIN32 OR WIN64)) )
    add_executable(atb-agg  atb-agg.c)
    target_link_libraries( atb-agg freetype-gl)
    target_link_libraries( atb-agg ${GLFW3_LIBRARY})
    target_link_libraries( atb-agg ${GLEW_LIBRARY})
    target_link_libraries( atb-agg ${FREETYPE_LIBRARIES})
    target_link_libraries( atb-agg ${ANT_TWEAK_BAR_LIBRARY})
    target_link_libraries( atb-agg ${OPENGL_LIBRARY})
    IF(MATH_LIBRARY)
        target_link_libraries( atb-agg ${MATH_LIBRARY})
    ENDIF(MATH_LIBRARY)
ENDIF (ANT_TWEAK_BAR_FOUND AND (NOT (WIN32 OR WIN64)) )

# Copy font and shaders into build directory for in-place testing
file(COPY ../fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
