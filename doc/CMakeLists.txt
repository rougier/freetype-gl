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

find_package(Doxygen REQUIRED)

set(FREETYPE_GL_APIDOC_SRC
    Doxyfile.in
    documentation.h
    doxygen.css
    footer.html
    header.html
    layout.xml
    screenshots.h
    style.css
    text-rendering.h
)

file(GLOB FREETYPE_GL_APIDOC_IMG images/*)
file(GLOB FREETYPE_GL_SRC ../*.h ../*.c)
file(GLOB FREETYPE_GL_DEMOS ../demos/*.c)

configure_file(Doxyfile.in Doxyfile)

add_custom_command(
    OUTPUT
        html/index.html
    COMMAND
        ${DOXYGEN_EXECUTABLE} Doxyfile
    DEPENDS
        ${FREETYPE_GL_APIDOC_SRC}
        ${FREETYPE_GL_APIDOC_IMG}
        ${FREETYPE_GL_SRC}
        ${FREETYPE_GL_DEMOS}
)

add_custom_target(doc
    ALL
    DEPENDS html/index.html
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)

set_directory_properties(
    PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/html
)
