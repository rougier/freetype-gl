/* =========================================================================
 * Freetype GL - An ansi C OpenGL Freetype engine
 * Platform:    Any
 * API version: 1.0
 * WWW:         http://www.loria.fr/~rougier/freetype-gl
 * -------------------------------------------------------------------------
 * Copyright (c) 2011 Nicolas P. Rougier <Nicolas.Rougier@inria.fr>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 * ========================================================================= */
#pragma once
#ifndef __VERTEX_ATTRIBUTE_H__
#define __VERTEX_ATTRIBUTE_H__

#define MAX_VERTEX_ATTRIBUTE 8


struct __VertexAttribute__
{
    GLenum   target;
    GLint    size;
    GLenum   type;
    GLsizei  stride;
    GLvoid * pointer;
    void ( * enable )(struct __VertexAttribute__ *);
};

typedef struct __VertexAttribute__ VertexAttribute;

VertexAttribute *
vertex_attribute_new( GLenum target,
                      GLint size,
                      GLenum type,
                      GLsizei stride,
                      GLvoid *pointer );
VertexAttribute *vertex_attribute_parse( char *format );
void vertex_attribute_position_enable( VertexAttribute *attr );
void vertex_attribute_normal_enable( VertexAttribute *attr );
void vertex_attribute_color_enable( VertexAttribute *attr );
void vertex_attribute_tex_coord_enable( VertexAttribute *attr );
void vertex_attribute_fog_coord_enable( VertexAttribute *attr );
void vertex_attribute_edge_flag_enable( VertexAttribute *attr );
void vertex_attribute_secondary_color_enable( VertexAttribute *attr );


GLenum GL_TYPE( char ctype );
GLenum GL_VERTEX_ATTRIBUTE_TARGET( char ctarget );
GLuint GL_TYPE_SIZE( GLenum gtype );
char * GL_TYPE_STRING( GLenum gtype );


#endif /* __VERTEX_ATTRIBUTE_H__ */
