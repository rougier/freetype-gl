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

MACRO( DEMO _target _sources)
     add_executable(${_target} ${_sources})
     include_directories(${WINDOWS_DIR} ${GLUT_INCLUDE_DIR} ${GLEW_INCLUDE_PATH} ${FREETYPE_INCLUDE_DIR_ft2build} ${FREETYPE_INCLUDE_DIR_freetype2} ${ANT_TWEAK_BAR_INCLUDE_PATH})
    if(NOT APPLE)
     target_link_libraries(${_target} ${GLEW_LIBRARY})
    endif()
     target_link_libraries(${_target} freetype-gl-hb)
     target_link_libraries(${_target} ${OPENGL_LIBRARY})
     target_link_libraries(${_target} ${GLUT_LIBRARY})
     target_link_libraries(${_target} ${FREETYPE_LIBRARY})
     target_link_libraries(${_target} ${HARFBUZZ_LIBRARIES})
     IF( MATH_LIBRARY )
         target_link_libraries(${_target} ${MATH_LIBRARY})
     ENDIF( MATH_LIBRARY )
    if(MSVC)
      # Compile demos as C++
      set_source_files_properties(${_sources} PROPERTIES LANGUAGE CXX)
    endif()
ENDMACRO( DEMO )

FIND_PACKAGE( HarfBuzz REQUIRED )

SET( FREETYPE_GL_HB_SRC freetype-gl.h
                        vec234.h
                        opengl.h
                        mat4.c             mat4.h
                        texture-atlas.c    texture-atlas.h
                        texture-font.c     texture-font.h
                        vertex-buffer.c    vertex-buffer.h
                        vertex-attribute.c vertex-attribute.h
                        shader.c           shader.h
                        vector.c           vector.h)

INCLUDE_DIRECTORIES( ${HARFBUZZ_INCLUDE_DIRS} )

ADD_LIBRARY( freetype-gl-hb STATIC ${FREETYPE_GL_HB_SRC} )

DEMO( harfbuzz          "demo-harfbuzz.c" )
DEMO( hb-texture        "demo-texture.c" )

# Copy font and shaders into build directory for in-place testing
file(COPY ../fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
