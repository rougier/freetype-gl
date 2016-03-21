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

project(freetype-gl)

set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules"
    "${CMAKE_MODULE_PATH}"
)

option(freetype-gl_USE_VAO "Use a VAO to render a vertex_buffer instance (required for forward compatible OpenGL 3.0 contexts)" OFF)
option(freetype-gl_BUILD_DEMOS "Build the freetype-gl example programs" ON)
option(freetype-gl_BUILD_APIDOC "Build the freetype-gl API documentation" ON)
option(freetype-gl_BUILD_HARFBUZZ "Build the freetype-gl harfbuzz support (experimental)" OFF)
option(freetype-gl_BUILD_MAKEFONT "Build the makefont tool" ON)

if(WIN32 OR WIN64)
    set(GLEW_ROOT_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/glew)
    set(ENV{FREETYPE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/freetype)
    set(VS789FIX_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/include)
else()
    find_library(MATH_LIBRARY m)
endif()

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

if(NOT APPLE)
    find_package(GLEW REQUIRED)
endif()

include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${VS789FIX_INCLUDE_DIR}
    ${GLEW_INCLUDE_PATH}
)

if(MSVC)
    # _CRT_NONSTDC_NO_DEPRECATE -> remove warning C4996: 'strdup': The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: _strdup.
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
endif(MSVC)

if(freetype-gl_USE_VAO)
    add_definitions(-DFREETYPE_GL_USE_VAO)
endif(freetype-gl_USE_VAO)

set(FREETYPE_GL_HDR
    distance-field.h
    edtaa3func.h
    font-manager.h
    freetype-gl.h
    markup.h
    mat4.h
    opengl.h
    platform.h
    shader.h
    text-buffer.h
    texture-atlas.h
    texture-font.h
    utf8-utils.h
    vec234.h
    vector.h
    vertex-attribute.h
    vertex-buffer.h
)

set(FREETYPE_GL_SRC
    distance-field.c
    edtaa3func.c
    font-manager.c
    mat4.c
    platform.c
    shader.c
    text-buffer.c
    texture-atlas.c
    texture-font.c
    utf8-utils.c
    vector.c
    vertex-attribute.c
    vertex-buffer.c
)

add_library(freetype-gl STATIC
    ${FREETYPE_GL_SRC}
    ${FREETYPE_GL_HDR}
)

if(freetype-gl_BUILD_MAKEFONT)
    add_executable(makefont makefont.c)

    if(NOT APPLE)
        target_link_libraries(makefont ${GLEW_LIBRARY})
    endif()

    target_link_libraries(makefont
        freetype-gl
        ${OPENGL_LIBRARY}
        ${FREETYPE_LIBRARIES}
        ${MATH_LIBRARY}
    )

    if(MSVC)
        # Compile makefont as C++
        set_source_files_properties(makefont.c PROPERTIES LANGUAGE CXX)
    endif()
endif()

if(freetype-gl_BUILD_APIDOC)
    add_subdirectory(doc)
endif()

if(freetype-gl_BUILD_DEMOS)
    add_subdirectory(demos)
endif()

if(freetype-gl_BUILD_HARFBUZZ)
    add_subdirectory(harfbuzz)
endif()
