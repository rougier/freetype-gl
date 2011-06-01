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
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "vertex-buffer.h"
#include "vertex-attribute.h"


VertexBuffer *
vertex_buffer_new( char *format )
{
    size_t i, index = 0, stride = 0;
    char *p = format-1;
    GLvoid *pointer = 0;

    VertexBuffer *self = (VertexBuffer *) malloc (sizeof(VertexBuffer));
    if( !self )
    {
        return NULL;
    }

    self->format = strdup( format );

    for( i=0; i<MAX_VERTEX_ATTRIBUTE; ++i )
    {
        self->attributes[i] = 0;
    }

    while ( p && (index < MAX_VERTEX_ATTRIBUTE) )
    {
        p++;
        VertexAttribute *attribute = vertex_attribute_parse( p );
        p = (char *) (strchr(p, ':'));
        attribute->pointer = pointer;
        stride += attribute->size*GL_TYPE_SIZE( attribute->type );
        pointer+= attribute->size*GL_TYPE_SIZE( attribute->type );
        self->attributes[index] = attribute;
        index++;
    }
    for( i=0; i<index; ++i )
    {
        self->attributes[i]->stride = stride;
    }
    self->vertices = vector_new( stride );
    self->vertices_id  = 0;
    self->indices = vector_new( sizeof(GLuint) );
    self->indices_id  = 0;
    return self;
}



VertexBuffer *
vertex_buffer_new_from_data( char *format,
                             size_t vcount, void   *vertices,
                             size_t icount, GLuint *indices )
{
    VertexBuffer *self = vertex_buffer_new( format );

    vector_resize( self->vertices, vcount );
    assert( self->vertices->size == vcount);
    memcpy( self->vertices->items, vertices, vcount*self->vertices->item_size );
    vector_resize( self->indices, icount );
    assert( self->indices->size == icount);
    memcpy( self->indices->items, indices, icount*self->indices->item_size );
    self->dirty = 1;
    return self;
}

void
vertex_buffer_delete( VertexBuffer *self )
{
    assert( self );

    vector_delete( self->vertices );
    self->vertices = 0;
    if( self->vertices_id )
    {
        glDeleteBuffers( 1, &self->vertices_id );
    }
    self->vertices_id = 0;

    vector_delete( self->indices );
    self->indices = 0;
    if( self->indices_id )
    {
        glDeleteBuffers( 1, &self->vertices_id );
    }
    self->indices_id = 0;
    if( self->format )
    {
        free( self->format );
    }
    self->format = 0;
    self->dirty = 0;
    free( self );
}

void
vertex_buffer_upload ( VertexBuffer *self )
{
    if( !self->vertices_id )
    {
        glGenBuffers( 1, &self->vertices_id );
    }
    if( !self->indices_id )
    {
        glGenBuffers( 1, &self->indices_id );
    }
    glBindBuffer( GL_ARRAY_BUFFER, self->vertices_id );
    glBufferData( GL_ARRAY_BUFFER, self->vertices->size*self->vertices->item_size,
                                   self->vertices->items, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, self->indices_id );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, self->indices->size*self->indices->item_size,
                                           self->indices->items, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void
vertex_buffer_render ( VertexBuffer *self, GLenum mode, char *what )
{ 
    int i,j;

    if( self->dirty )
    {
        vertex_buffer_upload( self );
        self->dirty = 0;
    }
    
    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
    glBindBuffer( GL_ARRAY_BUFFER, self->vertices_id );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, self->indices_id );
    for (i=0; i<strlen(what); ++i)
    {
        char ctarget = what[i];
        GLuint target = GL_VERTEX_ATTRIBUTE_TARGET( ctarget );
        for (j=0; j<MAX_VERTEX_ATTRIBUTE; ++j)
        {
            VertexAttribute *attribute = self->attributes[j];
            if ( attribute == 0 )
            {
                break;
            }
            else if (attribute->target == target)
            {
                (*(attribute->enable))( attribute );
                break;
            }
        }
    }
    glDrawElements( mode, self->indices->size, GL_UNSIGNED_INT, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glPopClientAttrib( );
}

void
vertex_buffer_push_back_index ( VertexBuffer *self,
                                GLuint index )
{
    self->dirty = 1;
    vector_push_back( self->indices, &index );
}

void
vertex_buffer_push_back_vertex ( VertexBuffer *self,
                                 void *vertex )
{
    self->dirty = 1;
    vector_push_back( self->vertices, vertex );
}

void
vertex_buffer_push_back_indices ( VertexBuffer *self,
                                  GLuint *indices,
                                  size_t count )
{
    self->dirty = 1;
    vector_push_back_data( self->indices, indices, count );
}

void
vertex_buffer_push_back_vertices ( VertexBuffer *self,
                                   void *vertices,
                                   size_t count )
{
    self->dirty = 1;
    vector_push_back_data( self->vertices, vertices, count );

}

void
vertex_buffer_insert_indices ( VertexBuffer *self,
                               size_t index,
                               GLuint *indices,
                               size_t count )
{
    self->dirty = 1;
    vector_insert_data( self->indices, index, indices, count );
}

void
vertex_buffer_insert_vertices ( VertexBuffer *self,
                                size_t index,
                                void *vertices,
                                size_t count )
{
    size_t i;

    assert( self );
    assert( self->vertices );
    assert( index < self->vertices->size+1 );

    self->dirty = 1;
    for( i=0; i<self->indices->size-index; ++i )
    {
        if( *(GLuint *)(vector_get( self->indices, i )) > index )
        {
            *(GLuint *)(vector_get( self->indices, i )) += index;
        }
    }
    vector_insert_data( self->vertices, index, vertices, count );
}
