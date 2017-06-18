# Freetype GL - A C OpenGL Freetype engine
#
# Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
# file `LICENSE` for more details.

find_package(HarfBuzz REQUIRED)

include_directories(
    ${HARFBUZZ_INCLUDE_DIRS}
)

set(FREETYPE_GL_HB_HDR
    freetype-gl.h
    opengl.h
    texture-atlas.h
    texture-font.h
    vec234.h
    vector.h
    vertex-attribute.h
    vertex-buffer.h
    freetype-gl-err.h
    freetype-gl-errdef.h
)

set(FREETYPE_GL_HB_SRC
    texture-atlas.c
    texture-font.c
    vector.c
    vertex-attribute.c
    vertex-buffer.c
    freetype-gl-err.c
)

if(freetype-gl_BUILD_SHARED)
    add_library(freetype-gl-hb SHARED
        ${FREETYPE_GL_HB_SRC}
        ${FREETYPE_GL_HB_HDR}
    )
    SET_TARGET_PROPERTIES(
        freetype-gl-hb
    PROPERTIES
        VERSION 0.3.2
        SOVERSION 0)
    target_link_libraries (freetype-gl-hb
			   ${OPENGL_LIBRARY}
			   ${FREETYPE_LIBRARIES}
			   ${MATH_LIBRARY}
			   ${GLEW_LIBRARY}
			   )
else()
    add_library(freetype-gl-hb STATIC
        ${FREETYPE_GL_HB_SRC}
        ${FREETYPE_GL_HB_HDR}
    )
endif()

