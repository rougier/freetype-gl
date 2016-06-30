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
cmake_minimum_required(VERSION 2.8.5)

macro(CREATE_DEMO _target _sources)
    add_executable(${_target} ${_sources})
    target_link_libraries(${_target}
        freetype-gl-hb
        ${OPENGL_LIBRARY}
        ${GLFW3_LIBRARY}
        ${FREETYPE_LIBRARIES}
        ${HARFBUZZ_LIBRARIES}
        ${GLEW_LIBRARY}
     )
     if(MATH_LIBRARY)
         target_link_libraries(${_target} ${MATH_LIBRARY})
     endif()
    if(MSVC)
      # Compile demos as C++
      set_source_files_properties(${_sources} PROPERTIES LANGUAGE CXX)
    endif()
endmacro()

find_package(glfw3 REQUIRED)
find_package(HarfBuzz REQUIRED)

include_directories(
    ${GLFW3_INCLUDE_DIR}
    ${HARFBUZZ_INCLUDE_DIRS}
)

link_directories(
    ${GLFW3_LIBRARY_DIR}
)

set(FREETYPE_GL_HB_HDR
    freetype-gl.h
    mat4.h
    opengl.h
    shader.h
    texture-atlas.h
    texture-font.h
    vec234.h
    vector.h
    vertex-attribute.h
    vertex-buffer.h
)

set(FREETYPE_GL_HB_SRC
    mat4.c
    shader.c
    texture-atlas.c
    texture-font.c
    vector.c
    vertex-attribute.c
    vertex-buffer.c
)

add_library(freetype-gl-hb STATIC
    ${FREETYPE_GL_HB_SRC}
    ${FREETYPE_GL_HB_HDR}
)

create_demo(harfbuzz demo-harfbuzz.c)
create_demo(hb-texture demo-texture.c)

# Copy font and shaders into build directory for in-place testing
file(COPY ../fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
