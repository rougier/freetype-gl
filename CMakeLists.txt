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
SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules"
                       "${CMAKE_MODULE_PATH}" )

OPTION(freetype-gl_BUILD_DEMOS "Build the freetype-gl example programs" ON)

# Get required and optional library
FIND_PACKAGE( OpenGL REQUIRED )
IF( WIN32 OR WIN64 )
	SET( WINDOWS_DIR
         ${CMAKE_CURRENT_SOURCE_DIR}/windows )
	SET( GLUT_INCLUDE_DIR
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/glut )
	SET( GLUT_LIBRARY
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/glut/glut32.lib )
    SET( GLEW_INCLUDE_PATH
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/glew/include )
    SET( GLEW_LIBRARY
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/glew/lib/glew32.lib )
	SET( FREETYPE_INCLUDE_DIRS
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/freetype )
	SET( FREETYPE_INCLUDE_DIR_ft2build
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/freetype )
	SET( FREETYPE_INCLUDE_DIR_freetype2
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/freetype/freetype )
	SET( FREETYPE_LIBRARY
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/freetype/freetype.lib )
	SET( ANT_TWEAK_BAR_INCLUDE_PATH
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/AntTweakBar/include )
	SET( ANT_TWEAK_BAR_LIBRARY
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/AntTweakBar/lib/AntTweakBar.lib )
    SET( VS789FIX_INCLUDE_DIR
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/include )
    SET( ANT_TWEAK_BAR_FOUND "YES" )
ELSE( WIN32 OR WIN64 )
    FIND_PACKAGE( GLUT REQUIRED )
    FIND_PACKAGE( Freetype REQUIRED )
    FIND_PACKAGE( GLEW REQUIRED )
    FIND_PACKAGE( FontConfig )
    FIND_LIBRARY( MATH_LIBRARY m )
    #FIND_LIBRARY( STDC_LIBRARY stdc++) #Buggy Cmake can't find libstdc++
    SET( ANT_TWEAK_BAR_INCLUDE_PATH "")
    FIND_PACKAGE( AntTweakBar )
ENDIF( WIN32 OR WIN64 )

INCLUDE_DIRECTORIES( ${GLUT_INCLUDE_DIR}
                     ${OPENGL_INCLUDE_DIRS}
                     ${FREETYPE_INCLUDE_DIRS}
                     ${GLEW_INCLUDE_PATHS}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${VS789FIX_INCLUDE_DIR}
                     ${GLEW_INCLUDE_PATH})
					 
IF(MSVC)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

SET( FREETYPE_GL_SRC freetype-gl.h
                     vec234.h
                     opengl.h
                     markup.h
                     mat4.c             mat4.h
                     texture-atlas.c    texture-atlas.h
                     texture-font.c     texture-font.h
                     vertex-buffer.c    vertex-buffer.h
                     vertex-attribute.c vertex-attribute.h
                     font-manager.c     font-manager.h
                     text-buffer.c      text-buffer.h
                     shader.c           shader.h
                     vector.c           vector.h
                     platform.c         platform.h)

ADD_LIBRARY( freetype-gl STATIC ${FREETYPE_GL_SRC} )

LINK_DIRECTORIES(${PROJECT_SOURCE_DIR})

MACRO( DEMO _target _sources)
     add_executable(${_target} ${_sources})
     include_directories(${WINDOWS_DIR} ${GLUT_INCLUDE_DIR} ${GLEW_INCLUDE_PATH} ${FREETYPE_INCLUDE_DIR_ft2build} ${FREETYPE_INCLUDE_DIR_freetype2} ${ANT_TWEAK_BAR_INCLUDE_PATH})
     target_link_libraries(${_target} ${GLEW_LIBRARY})
     target_link_libraries(${_target} freetype-gl)
     target_link_libraries(${_target} ${OPENGL_LIBRARY})
     target_link_libraries(${_target} ${GLUT_LIBRARY})
     target_link_libraries(${_target} ${GLEW_LIBRARY})
     target_link_libraries(${_target} ${FREETYPE_LIBRARY})
     IF( MATH_LIBRARY )
         target_link_libraries(${_target} ${MATH_LIBRARY})
     ENDIF( MATH_LIBRARY )
    if(MSVC)
      # Compile demos as C++
      set_source_files_properties(${_sources} PROPERTIES LANGUAGE CXX)
    endif()
ENDMACRO( DEMO )

IF (freetype-gl_BUILD_DEMOS)

DEMO( demo-texture          "demo-texture.c" )
DEMO( demo-font             "demo-font.c" )
DEMO( demo-benchmark        "demo-benchmark.c" )
DEMO( demo-console          "demo-console.c" )
DEMO( demo-cube             "demo-cube.c" )
DEMO( demo-glyph            "demo-glyph.c" )
DEMO( demo-cartoon          "demo-cartoon.c" )
DEMO( demo-outline          "demo-outline.c" )
DEMO( demo-lcd              "demo-lcd.c" )
DEMO( demo-gamma            "demo-gamma.c" )
DEMO( demo-ansi             "demo-ansi.c" )
DEMO( demo-subpixel         "demo-subpixel.c" )
DEMO( demo-make             "demo-makefont.c" )
DEMO( makefont              "makefont.c" )
DEMO( demo-distance-field   "demo-distance-field.c;edtaa3func.c")
DEMO( demo-distance-field-2 "demo-distance-field-2.c;edtaa3func.c")
DEMO( demo-distance-field-3 "demo-distance-field-3.c;edtaa3func.c")

IF (FONTCONFIG_FOUND)
   INCLUDE_DIRECTORIES( ${FONTCONFIG_INCLUDE_DIR} )
   DEMO( demo-markup      "demo-markup.c" )
   target_link_libraries(demo-markup ${FONTCONFIG_LIBRARY})
ENDIF (FONTCONFIG_FOUND)

IF (ANT_TWEAK_BAR_FOUND AND (NOT (WIN32 OR WIN64)) )  
 #DEMO( demo-atb-agg     "demo-atb-agg.c;markup.h;")
 FILE(WRITE ${CMAKE_SOURCE_DIR}/stdc_test.c
  "int main(void){return 0;}\n")
  EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -lstdc++ OUTPUT_VARIABLE out_res ERROR_VARIABLE err_res)
  string(FIND VAL"${err_res}" "main" substr_index ) 
  if(${substr_index} STRGREATER "0")
      message (STATUS "Found Libstdc++ needed by demo-atb-agg.c ." )

      add_executable(demo-atb-agg  demo-atb-agg.c) 
      target_link_libraries( demo-atb-agg freetype-gl)
      target_link_libraries( demo-atb-agg ${GLUT_LIBRARY})
      target_link_libraries( demo-atb-agg ${GLEW_LIBRARY})
      target_link_libraries( demo-atb-agg ${FREETYPE_LIBRARY})
      target_link_libraries( demo-atb-agg ${ANT_TWEAK_BAR_LIBRARY})
      # target_link_libraries( demo-atb-agg ${STDC_LIBRARY}) # Buggy Cmake can't find libstdc++
      # quirk: because of link order libstdc++ must be linked before libGLU
      target_link_libraries( demo-atb-agg "-lstdc++")
      target_link_libraries( demo-atb-agg ${OPENGL_LIBRARY})
      IF(MATH_LIBRARY)
        target_link_libraries( demo-atb-agg ${MATH_LIBRARY})
      ENDIF(MATH_LIBRARY) 
  else()
      message (STATUS "NOT FOUND: Libstdc++ is not installed. It is needed by demo-atb-agg.c" )
  endif()
  FILE(REMOVE ${CMAKE_SOURCE_DIR}/stdc_test.c)
ENDIF (ANT_TWEAK_BAR_FOUND AND (NOT (WIN32 OR WIN64)) )  

# Copy font and shaders into build directory for in-place testing
FILE(GLOB Fonts   "fonts/Vera.ttf"
                  "fonts/VeraMoBI.ttf"
                  "fonts/VeraMoBd.ttf"
                  "fonts/VeraMoIt.ttf"
                  "fonts/VeraMono.ttf"
                  "fonts/ObelixPro.ttf")
file(COPY ${Fonts}   DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/fonts)

FILE(GLOB Data   "data/256colors.txt")
file(COPY ${Data}   DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)

FILE(GLOB Shaders "shaders/*.vert" "shaders/*.frag")
file(COPY ${Shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)

ENDIF (freetype-gl_BUILD_DEMOS)
