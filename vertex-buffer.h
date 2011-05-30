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
#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__

#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include "vector.h"
#include "vertex-attribute.h"

/**
 *
 */



struct __VertexBuffer__
{
    char *   format;
    vector * vertices;
    GLuint   vertices_id;
    vector * indices;
    GLuint   indices_id;
    char     dirty;
    VertexAttribute *attributes[MAX_VERTEX_ATTRIBUTE];
};

typedef struct __VertexBuffer__ VertexBuffer;


VertexBuffer *vertex_buffer_new( char *format );
VertexBuffer *vertex_buffer_new_from_data( char *format,
                                           size_t vcount, void   *vertices,
                                           size_t icount, GLuint *indices );
void          vertex_buffer_delete( VertexBuffer *self );
void          vertex_buffer_upload( VertexBuffer *self );
void          vertex_buffer_render ( VertexBuffer *self,
                                     GLenum mode, char *what );
void          vertex_buffer_push_back_index ( VertexBuffer *self,
                                              GLuint index );
void          vertex_buffer_push_back_vertex ( VertexBuffer *self,
                                               void *vertex );
void          vertex_buffer_push_back_indices ( VertexBuffer *self,
                                                GLuint *indices,
                                                size_t count );
void          vertex_buffer_push_back_vertices ( VertexBuffer *self,
                                                 void *vertices,
                                                 size_t count );
void          vertex_buffer_insert_indices ( VertexBuffer *self,
                                             size_t index,
                                             GLuint *indices,
                                             size_t count );
void          vertex_buffer_add_vertices ( VertexBuffer *self,
                                             size_t index,
                                           void *vertices,
                                           size_t count );

#endif /* __VERTEX_BUFFER_H__ */
