/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * -------------------------------------------------------------------------
 * Copyright 2011 Nicolas P. Rougier. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Nicolas P. Rougier.
 * ========================================================================= */
#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include "vector.h"

#define MAX_VERTEX_ATTRIBUTE 64



/**
 *  Generic vertex attribute.
 */
typedef struct 
{
    /**
     *  a client-side capability.
     */
    GLenum target;

    /**
     *  a translated client-side capability.
     */
    GLchar ctarget;

    /**
     * index of the generic vertex attribute to be modified.
     */
    GLuint index;

    /**
     * Number of components per generic vertex attribute. Must be 1, 2, 3, or
     * 4. The initial value is 4.
     */
    GLint size;

    /** 
     *  Data type of each component in the array. Symbolic constants GL_BYTE,
     *  GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT,
     *  GL_FLOAT, or GL_DOUBLE are accepted. The initial value is GL_FLOAT.
     */
    GLenum type;

    /**
     *  Whether fixed-point data values should be normalized (GL_TRUE) or
     *  converted directly as fixed-point values (GL_FALSE) when they are
     *  accessed.
     */
    GLboolean normalized;

    /**
     *  Byte offset between consecutive generic vertex attributes. If stride is
     *  0, the generic vertex attributes are understood to be tightly packed in
     *  the array. The initial value is 0.
     */
    GLsizei stride;

    /**
     *  Pointer to the first component of the first attribute element in the
     *  array.
     */
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

    /** GL identity of the vertices buffer. */
    GLuint vertices_id;

    /** Vector of indices. */
    Vector * indices;

    /** GL identity of the indices buffer. */
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
 * Print information about a vertex buffer
 *
 * @param  self  a vertex buffer
 */
  void
  vertex_buffer_print( VertexBuffer * self );


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
 * Clear all vertices and indices
 *
 * @param  self  a vertex buffer
 */
  void
  vertex_buffer_clear( VertexBuffer *self );


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
 * @param target     client-side capability
 * @param index      index of the generic vertex attribute to be modified.
 * @param size       number of component
 * @param type       data type
 * @param normalized Whether fixed-point data values should be normalized
                     (GL_TRUE) or converted directly as fixed-point values
                     (GL_FALSE) when they are  accessed.
 * @param stride     byte offset between consecutive attributes.
 * @param pointer    pointer to the first component of the first attribute
 *                   element in the array.
 * @return           a new initialized vertex attribute.
 */
VertexAttribute *
vertex_attribute_new( GLenum target,
                      GLuint index,
                      GLint size,
                      GLenum type,
                      GLboolean normalized,
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
 * Enable a generic vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_generic_attribute_enable( VertexAttribute *attr );


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
