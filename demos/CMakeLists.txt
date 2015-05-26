# =========================================================================
# Freetype GL - A C OpenGL Freetype engine
# Platform:    Any
# WWW:         http://code.google.com/p/freetype-gl/
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

# Get required and optional library
IF( WIN32 OR WIN64 )
    SET( WINDOWS_DIR
         ${CMAKE_CURRENT_SOURCE_DIR}/../windows )
    SET( ANT_TWEAK_BAR_INCLUDE_PATH
         ${CMAKE_CURRENT_SOURCE_DIR}/../windows/AntTweakBar/include )
    SET( ANT_TWEAK_BAR_LIBRARY
         ${CMAKE_CURRENT_SOURCE_DIR}/../windows/AntTweakBar/lib/AntTweakBar.lib )
    SET( ANT_TWEAK_BAR_FOUND "YES" )
ELSE( WIN32 OR WIN64 )
    FIND_PACKAGE( FontConfig )
    #FIND_LIBRARY( STDC_LIBRARY stdc++) #Buggy Cmake can't find libstdc++
    FIND_PACKAGE( AntTweakBar )
ENDIF( WIN32 OR WIN64 )

MACRO( DEMO _target _sources)
     add_executable(${_target} ${_sources})
     include_directories(${WINDOWS_DIR} ${GLUT_INCLUDE_DIR} ${GLEW_INCLUDE_PATH} ${FREETYPE_INCLUDE_DIR_ft2build} ${FREETYPE_INCLUDE_DIR_freetype2} ${ANT_TWEAK_BAR_INCLUDE_PATH})
    if(NOT APPLE)
     target_link_libraries(${_target} ${GLEW_LIBRARY})
    endif()
     target_link_libraries(${_target} freetype-gl)
     target_link_libraries(${_target} ${OPENGL_LIBRARY})
     target_link_libraries(${_target} ${GLUT_LIBRARY})
     target_link_libraries(${_target} ${FREETYPE_LIBRARY})
     IF( MATH_LIBRARY )
         target_link_libraries(${_target} ${MATH_LIBRARY})
     ENDIF( MATH_LIBRARY )
    if(MSVC)
      # Compile demos as C++
      set_source_files_properties(${_sources} PROPERTIES LANGUAGE CXX)
    endif()
ENDMACRO( DEMO )

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
   INCLUDE_DIRECTORIES( ${FONTCONFIG_INCLUDE_DIR} )
   DEMO( markup      "markup.c" )
   target_link_libraries(markup ${FONTCONFIG_LIBRARY})
ENDIF (FONTCONFIG_FOUND)

IF (ANT_TWEAK_BAR_FOUND AND (NOT (WIN32 OR WIN64)) )
 #DEMO( atb-agg     "atb-agg.c;markup.h;")
 FILE(WRITE ${CMAKE_SOURCE_DIR}/stdc_test.c
  "int main(void){return 0;}\n")
  EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -lstdc++ OUTPUT_VARIABLE out_res ERROR_VARIABLE err_res)
  string(FIND VAL"${err_res}" "main" substr_index )
  if(${substr_index} STRGREATER "0")
      message (STATUS "Found Libstdc++ needed by atb-agg.c ." )

      add_executable(atb-agg  atb-agg.c)
      target_link_libraries( atb-agg freetype-gl)
      target_link_libraries( atb-agg ${GLUT_LIBRARY})
      target_link_libraries( atb-agg ${GLEW_LIBRARY})
      target_link_libraries( atb-agg ${FREETYPE_LIBRARY})
      target_link_libraries( atb-agg ${ANT_TWEAK_BAR_LIBRARY})
      # target_link_libraries( atb-agg ${STDC_LIBRARY}) # Buggy Cmake can't find libstdc++
      # quirk: because of link order libstdc++ must be linked before libGLU
      target_link_libraries( atb-agg "-lstdc++")
      target_link_libraries( atb-agg ${OPENGL_LIBRARY})
      IF(MATH_LIBRARY)
        target_link_libraries( atb-agg ${MATH_LIBRARY})
      ENDIF(MATH_LIBRARY)
  else()
      message (STATUS "NOT FOUND: Libstdc++ is not installed. It is needed by atb-agg.c" )
  endif()
  FILE(REMOVE ${CMAKE_SOURCE_DIR}/stdc_test.c)
ENDIF (ANT_TWEAK_BAR_FOUND AND (NOT (WIN32 OR WIN64)) )

# Copy font and shaders into build directory for in-place testing
file(COPY ../fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
