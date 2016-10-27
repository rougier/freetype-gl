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

#.rst:
# RequireIncludeFile
# ------------------
#
# Provides a macro to require the existence of a ``C`` header.
#
# .. command:: REQUIRE_INCLUDE_FILE
#
#  ::
#
#    REQUIRE_INCLUDE_FILE(<include> VARIABLE [<FLAGS>])
#
#  Uses check_include_file internally and aborts the configure process with a
#  Fatal error when the header was not found.
#
# See the :module:`CheckIncludeFile` module for more details.

include(CheckIncludeFile)

macro(REQUIRE_INCLUDE_FILE INCLUDE_FILE VARIABLE)
    if(${ARGC} EQUAL 3)
        check_include_file(${INCLUDE_FILE} ${VARIABLE} ${ARGV2})
    else()
        check_include_file(${INCLUDE_FILE} ${VARIABLE})
    endif()

    if(NOT ${VARIABLE})
        message(FATAL_ERROR "Required `${INCLUDE_FILE}` include file not found")
    endif()
endmacro()
