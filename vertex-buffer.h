/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * API version: 1.0
 * WWW:         http://code.google.com/p/freetype-gl/
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 * =========================================================================

   ========================================================================= */
#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include "vector.h"

#define MAX_VERTEX_ATTRIBUTE 8



/**
 *  Generic vertex attribute.
 */
typedef struct 
{
    /** a client-side capability. */
    GLenum target;

    /** Number of component. */
    GLint size;

    /** Data type. */
    GLenum type;

    /** Byte offset between consecutive attributes. */
    GLsizei stride;

    /** Pointer to the first component of the first attribute element in the
     *  array. */
    GLvoid * pointer;

    /** Pointer to the function that enable this attribute. */
    void ( * enable )(void *);
} VertexAttribute;



/**
 * Generic vertex buffer.
 */
typedef struct
{
    /** Format of the vertex buffer. */
    char * format;

    /** Vector of vertices. */
    Vector * vertices;

    /** GL idendity of the vertices buffer. */
    GLuint vertices_id;

    /** Vector of indices. */
    Vector * indices;

    /** GL idendity of the indices buffer. */
    GLuint indices_id;

    /** Whether the vertex buffer needs to be uploaded to GPU memory. */
    char dirty;

    /** Array of attributes. */
    VertexAttribute *attributes[MAX_VERTEX_ATTRIBUTE];
} VertexBuffer;



/**
 * Creates an empty vertex buffer.
 *
 * @param  format a string describing vertex format.
 * @return        an empty vertex buffer.
 */
  VertexBuffer *
  vertex_buffer_new( char *format );


/**
 * Creates a vertex buffer from data.
 *
 * @param  format   a string describing vertex format.
 * @param  vcount   number of vertices
 * @param  vertices raw vertices data
 * @param  icount   number of vertices
 * @param  indices  raw indices data
 * @return          an empty vertex buffer.
 */
  VertexBuffer *
  vertex_buffer_new_from_data( char *format,
                               size_t vcount,
                               void * vertices,
                               size_t icount,
                               GLuint * indices );


/**
 * Deletes vertex buffer and releases GPU memory.
 *
 * @param  self  a vertex buffer
 */
  void
  vertex_buffer_delete( VertexBuffer * self );


/**
 * Immediate draw
 *
 * @param  self  a vertex buffer
 * @param  mode  render mode
 * @param  what  attributes to be rendered
 */
  void
  vertex_buffer_draw ( const char * format,
                       const GLenum mode,
                       const void * vertices,
                       const size_t count );


/**
 * Immediate draw with indexed vertices
 *
 * @param  self  a vertex buffer
 * @param  mode  render mode
 * @param  what  attributes to be rendered
 */
  void
  vertex_buffer_draw_indexed ( const char * format,
                               const GLenum mode,
                               const void * vertices,
                               const size_t vcount,
                               const GLuint * indices,
                               const size_t icount );


/**
 * Render vertex buffer.
 *
 * @param  self  a vertex buffer
 * @param  mode  render mode
 * @param  what  attributes to be rendered
 */
  void
  vertex_buffer_render ( VertexBuffer *self,
                         GLenum mode,
                         char *what );


/**
 * Upload buffer to GPU memory.
 *
 * @param  self  a vertex buffer
 */
  void
  vertex_buffer_upload( VertexBuffer *self );


/**
 * Appends a single index at the end of the buffer.
 *
 * @param  self  a vertex buffer
 * @param  index index to be appended
 */
  void
  vertex_buffer_push_back_index ( VertexBuffer *self,
                                  GLuint index );


/**
 * Appends a single vertex at the end of the buffer.
 *
 * @param  self   a vertex buffer
 * @param  vertex vertex to be appended
 */
  void
  vertex_buffer_push_back_vertex ( VertexBuffer *self,
                                   void *vertex );


/**
 * Appends indices at the end of the buffer.
 *
 * @param  self    a vertex buffer
 * @param  indices indices to be appended
 * @param  count   number of indices to be appended
 */
  void
  vertex_buffer_push_back_indices ( VertexBuffer *self,
                                    GLuint *indices,
                                    size_t count );


/**
 * Appends vertices at the end of the buffer.
 *
 * @param  self     a vertex buffer
 * @param  vertices vertices to be appended
 * @param  count    number of vertices to be appended
 */
  void
  vertex_buffer_push_back_vertices ( VertexBuffer *self,
                                     void *vertices,
                                     size_t count );


/**
 * Appends indices at the end of the buffer.
 *
 * @param  self    a vertex buffer
 * @param  index   location before which to insert indices
 * @param  indices indices to be appended
 * @param  count   number of indices to be appended
 */
  void
  vertex_buffer_insert_indices ( VertexBuffer *self,
                                 size_t index,
                                 GLuint *indices,
                                 size_t count );


/**
 * Appends indices at the end of the buffer.
 *
 * @param  self     a vertex buffer
 * @param  index    location before which to insert vertices
 * @param  vertices vertices to be appended
 * @param  count    number of vertices to be appended
 *
 * @note
 * Indices after index will be increased by count. 
 */
  void
  vertex_buffer_add_vertices ( VertexBuffer *self,
                               size_t index,
                               void *vertices,
                               size_t count );


/**
 * Create an attribute from the given parameters.
 *
 * @param  target  a client-side capability
 * @param  size    number of component
 * @param  type    data type
 * @param  stride  byte offset between consecutive attributes.
 * @param  pointer pointer to the first component of the first attribute
 *                 element in the array.
 * @return         a new initialized vertex attribute.
 */
VertexAttribute *
vertex_attribute_new( GLenum target,
                      GLint size,
                      GLenum type,
                      GLsizei stride,
                      GLvoid *pointer );


/**
 * Create an attribute from the given description.
 *
 * @param  format Format string specifies the format of a vertex attribute.
 * @return        an initialized vertex attribute
 *
 */
  VertexAttribute *
  vertex_attribute_parse( char *format );


/**
 * Enable the position vertex attribute.
 *
 * @param  attr a vertex attribute
 */
  void
  vertex_attribute_position_enable( VertexAttribute *attr );


/**
 * Enable the normal vertex attribute.
 *
 * @param  attr a vertex attribute
 */
  void
  vertex_attribute_normal_enable( VertexAttribute *attr );


/**
 * Enable the color vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_color_enable( VertexAttribute *attr );


/**
 * Enable the texture vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_tex_coord_enable( VertexAttribute *attr );


/**
 * Enable the fog vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_fog_coord_enable( VertexAttribute *attr );


/**
 * Enable the edge flag vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_edge_flag_enable( VertexAttribute *attr );


/**
 * Enable the secondary color vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_secondary_color_enable( VertexAttribute *attr );


/**
 * Returns the GL enum type correspond to given character.
 *
 * @param ctype  character type
 * @return       GL enum type
 */
  GLenum
  GL_TYPE( char ctype );


/**
 * Get the GL name of the given target.
 *
 * @param  ctarget  a char describing target ( one of v,c,e,f,n,s,t)
 * @return          the associated GL target
 */
  GLenum
  GL_VERTEX_ATTRIBUTE_TARGET( char ctarget );


/**
 * Returns the size of a given GL enum type.
 *
 * @param  gtype a GL enum type
 * @return       the size of the given type
 */
  GLuint
  GL_TYPE_SIZE( GLenum gtype );


/**
 * Returns the literal string of given GL enum type.
 *
 * @param  gtype a GL enum type
 * @return       the literal string describing the type
 */
  char *
  GL_TYPE_STRING( GLenum gtype );


#endif /* __VERTEX_BUFFER_H__ */
