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
    if(NOT APPLE)
        target_link_libraries(${_target} ${GLEW_LIBRARY})
    endif()
    target_link_libraries(${_target}
        freetype-gl
        ${OPENGL_LIBRARY}
        ${GLFW3_LIBRARY}
        ${FREETYPE_LIBRARIES}
     )
     if(MATH_LIBRARY)
         target_link_libraries(${_target} ${MATH_LIBRARY})
     endif()
     
    if(MSVC AND NOT (MSVC_VERSION LESS 1900))
        # prevent error LNK2019: unresolved external symbol _sprintf referenced in function __bdf_parse_properties
        # see http://stackoverflow.com/a/32418900/469659
        target_link_libraries(${_target} "legacy_stdio_definitions.lib")
    endif()
endmacro()

if(WIN32 OR WIN64)
    set(ENV{ANT_TWEAK_BAR_ROOT}
        ${CMAKE_CURRENT_SOURCE_DIR}/../windows/AntTweakBar
    )
endif()

# Find dependencies
find_package(glfw3 REQUIRED)
find_package(AntTweakBar)
find_package(FontConfig)

include_directories(
    ${GLFW3_INCLUDE_DIR}
    ${ANT_TWEAK_BAR_INCLUDE_PATH}
    ${FONTCONFIG_INCLUDE_DIR}
)

link_directories(
    ${GLFW3_LIBRARY_DIR}
)

add_custom_command(
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
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/vera-16.h
)

add_custom_target(generate-embedded-font
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/vera-16.h
)

create_demo(embedded-font embedded-font.c)
add_dependencies(embedded-font generate-embedded-font)

create_demo(texture texture.c)
create_demo(font font.c)
create_demo(benchmark benchmark.c)
create_demo(console console.c)
create_demo(cube cube.c)
create_demo(glyph glyph.c)
create_demo(cartoon cartoon.c)
create_demo(outline outline.c)
create_demo(lcd lcd.c)
create_demo(gamma gamma.c)
create_demo(ansi ansi.c)
create_demo(subpixel subpixel.c)
create_demo(distance-field distance-field.c)
create_demo(distance-field-2 distance-field-2.c)
create_demo(distance-field-3 distance-field-3.c)

if(FONTCONFIG_FOUND)
   create_demo(markup markup.c)
   target_link_libraries(markup ${FONTCONFIG_LIBRARY})
endif()

if(ANT_TWEAK_BAR_FOUND)
    create_demo(atb-agg atb-agg.c)
    target_link_libraries(atb-agg ${ANT_TWEAK_BAR_LIBRARY})
endif()

# Copy font and shaders into build directory for in-place testing
file(COPY ../fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
