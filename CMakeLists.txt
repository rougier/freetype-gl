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

OPTION(freetype-gl_USE_VAO "Use a VAO to render a vertex_buffer instance (required for forward compatible OpenGL 3.0 contexts)" OFF)
OPTION(freetype-gl_BUILD_DEMOS "Build the freetype-gl example programs" ON)
OPTION(freetype-gl_BUILD_APIDOC "Build the freetype-gl API documentation" ON)
OPTION(freetype-gl_BUILD_HARFBUZZ "Build the freetype-gl harfbuzz support (experimental)" OFF)
OPTION(freetype-gl_LIBS_SUPPLIED "Required libraries are supplied as part of parent build" OFF)

# Get required and optional library
FIND_PACKAGE( OpenGL REQUIRED )
IF( WIN32 OR WIN64 )
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
    SET( VS789FIX_INCLUDE_DIR
         ${CMAKE_CURRENT_SOURCE_DIR}/windows/include )
ELSE( WIN32 OR WIN64 )
    IF ( freetype-gl_LIBS_SUPPLIED )
        FIND_PACKAGE( GLUT )
        FIND_PACKAGE( Freetype )
        IF ( NOT APPLE )
            FIND_PACKAGE( GLEW )
        ENDIF()
    ELSE ( freetype-gl_LIBS_SUPPLIED )
        FIND_PACKAGE( GLUT REQUIRED )
        FIND_PACKAGE( Freetype REQUIRED )
        IF ( NOT APPLE )
            FIND_PACKAGE( GLEW REQUIRED )
        ENDIF()
    ENDIF ( freetype-gl_LIBS_SUPPLIED )

    FIND_LIBRARY( MATH_LIBRARY m )
ENDIF( WIN32 OR WIN64 )

INCLUDE_DIRECTORIES( ${GLUT_INCLUDE_DIR}
                     ${OPENGL_INCLUDE_DIRS}
                     ${FREETYPE_INCLUDE_DIRS}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${VS789FIX_INCLUDE_DIR}
                     ${GLEW_INCLUDE_PATH})
IF(MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

if( freetype-gl_USE_VAO )
    ADD_DEFINITIONS( -DFREETYPE_GL_USE_VAO )
ENDIF( freetype-gl_USE_VAO )

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
                     platform.c         platform.h
                     edtaa3func.c       edtaa3func.h)

ADD_LIBRARY( freetype-gl STATIC ${FREETYPE_GL_SRC} )

LINK_DIRECTORIES(${PROJECT_SOURCE_DIR})

ADD_EXECUTABLE( makefont makefont.c )

IF(NOT APPLE)
    TARGET_LINK_LIBRARIES( makefont ${GLEW_LIBRARY} )
ENDIF()

TARGET_LINK_LIBRARIES( makefont
                       freetype-gl
                       ${OPENGL_LIBRARY}
                       ${GLUT_LIBRARY}
                       ${FREETYPE_LIBRARY}
                       ${MATH_LIBRARY})

if(MSVC)
    # Compile makefont as C++
    set_source_files_properties( makefont.c PROPERTIES LANGUAGE CXX )
endif()

IF (freetype-gl_BUILD_APIDOC)
    ADD_SUBDIRECTORY(doc)
ENDIF (freetype-gl_BUILD_APIDOC)

IF (freetype-gl_BUILD_DEMOS)
    ADD_SUBDIRECTORY(demos)
ENDIF (freetype-gl_BUILD_DEMOS)

IF (freetype-gl_BUILD_HARFBUZZ)
    ADD_SUBDIRECTORY(harfbuzz)
ENDIF (freetype-gl_BUILD_HARFBUZZ)
