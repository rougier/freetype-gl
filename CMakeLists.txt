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

# Get required and optional library
FIND_PACKAGE( OpenGL REQUIRED )
IF( WIN32 )
	SET( GLUT_INCLUDE_DIR
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/glut )
	SET( GLUT_GLUT_LIBRARY
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/glut/glut32.lib )
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
    SET( ANT_TWEAK_BAR_FOUND "YES" )
ELSE( WIN32 )
    FIND_PACKAGE( GLUT REQUIRED )
    FIND_PACKAGE( Freetype REQUIRED )
    FIND_PACKAGE( FontConfig )
    FIND_PACKAGE( AntTweakBar )
ENDIF( WIN32 )

INCLUDE_DIRECTORIES( ${GLUT_INCLUDE_DIRS}
                     ${OPENGL_INCLUDE_DIRS}
                     ${FREETYPE_INCLUDE_DIRS}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

SET( FREETYPE_GL_LIBRARY freetype-gl )
SET(FREETYPE_GL_SRC freetype-gl.h   vec234.h
                    texture-atlas.c texture-atlas.h
                    texture-font.c  texture-font.h
                    vertex-buffer.c vertex-buffer.h
                    vector.c        vector.h
                    platform.c      platform.h)
ADD_LIBRARY( ${FREETYPE_GL_LIBRARY} STATIC ${FREETYPE_GL_SRC} )

MACRO( DEMO _target _sources)
     add_executable(${_target} ${_sources})
     target_link_libraries(${_target} ${OPENGL_LIBRARY})
     target_link_libraries(${_target} ${GLUT_LIBRARY})
     target_link_libraries(${_target} ${FREETYPE_LIBRARY})
     target_link_libraries(${_target} ${FREETYPE_GL_LIBRARY})
ENDMACRO( DEMO )

DEMO( demo-texture        "demo-texture.c" )
DEMO( demo-font           "demo-font.c" )
DEMO( demo-benchmark      "demo-benchmark.c" )
DEMO( demo-console        "demo-console.c" )
DEMO( demo-cube           "demo-cube.c" )
DEMO( demo-glyph          "demo-glyph.c" )
DEMO( demo-cartoon        "demo-cartoon.c" )
DEMO( demo-line-aa,       "demo-line-aa.c" )
DEMO( demo-gamma          "demo-gamma.c" )
DEMO( demo-outline        "demo-outline.c" )
DEMO( demo-display-list   "demo-display-list.c" )
DEMO( demo-lcd            "demo-lcd.c" )
DEMO( demo-line-aa        "demo-line-aa.c" )
DEMO( demo-subpixel       "demo-subpixel.c;subpixel.frag;subpixel.vert" )
DEMO( demo-make           "demo-makefont.c;arial-16.h" )
DEMO( makefont            "makefont.c" )
DEMO( demo-distance-field "demo-distance-field.c;edtaa3func.c;distance-field.frag;distance-field.vert")
IF (FONTCONFIG_FOUND)
   INCLUDE_DIRECTORIES( ${FONTCONFIG_INCLUDE_DIR} )
   DEMO( demo-markup      "demo-markup.c;font-manager.c;markup.vert;markup.frag" )
   target_link_libraries(demo-markup ${FONTCONFIG_LIBRARY})
ENDIF (FONTCONFIG_FOUND)

IF (ANT_TWEAK_BAR_FOUND)
   DEMO( demo-atb-agg     "demo-atb-agg.c;markup.c;markup.h;subpixel.vert;subpixel-gamma.frag")
   target_link_libraries( demo-atb-agg ${ANT_TWEAK_BAR_LIBRARY})
ENDIF (ANT_TWEAK_BAR_FOUND)

# Copy font and shaders into build directory for in-place testing
FILE(GLOB Fonts   "*.ttf")
file(COPY ${Fonts}   DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
FILE(GLOB Shaders "*.vert" "*.frag")
file(COPY ${Shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
