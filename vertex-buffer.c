/* ============================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * ----------------------------------------------------------------------------
 * Copyright 2011,2012 Nicolas P. Rougier. All rights reserved.
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
 * ============================================================================
 */
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vec234.h"
#include "vertex-buffer.h"


// If GL_DOUBLE does not exist we define it as GL_FLOAT
#ifndef GL_DOUBLE
#define GL_DOUBLE GL_FLOAT
#else
#define GL_DOUBLE_ GL_FLOAT
#endif 




// strndup() was only added in OSX lion
#ifdef __APPLE__

char *
strndup( const char *s1, size_t n)
{
    char *copy = calloc( n+1, sizeof(char) );
    memcpy( copy, s1, n );
    return copy;
};

#endif


// ----------------------------------------------------------------------------
vertex_buffer_t *
vertex_buffer_new( const char *format )
{
    size_t i, index = 0, stride = 0;
    const char *start = 0, *end = 0;
    GLchar *pointer = 0;

    vertex_buffer_t *self = (vertex_buffer_t *) malloc (sizeof(vertex_buffer_t));
    if( !self )
    {
        return NULL;
    }

    self->format = strdup( format );

    for( i=0; i<MAX_VERTEX_ATTRIBUTE; ++i )
    {
        self->attributes[i] = 0;
    }

    start = format;
    do
    {
        end = (char *) (strchr(start+1, ':'));
        char *desc = 0;
        if (end == NULL)
        {
            desc = strdup( start );
        }
        else
        {
            desc = strndup( start, end-start );
        }
        vertex_attribute_t *attribute = vertex_attribute_parse( desc );
        start = end+1;
        free(desc);
        attribute->pointer = pointer;
        stride  += attribute->size*GL_TYPE_SIZE( attribute->type );
        pointer += attribute->size*GL_TYPE_SIZE( attribute->type );
        self->attributes[index] = attribute;
        index++;
    } while ( end && (index < MAX_VERTEX_ATTRIBUTE) );

    for( i=0; i<index; ++i )
    {
        self->attributes[i]->stride = stride;
    }

    self->vertices = vector_new( stride );
    self->vertices_id  = 0;
    self->indices = vector_new( sizeof(GLuint) );
    self->indices_id  = 0;
    self->items = vector_new( sizeof(ivec4) );
    self->dirty = 1;
    self->mode = GL_TRIANGLES;
    return self;
}



// ----------------------------------------------------------------------------
void
vertex_buffer_delete( vertex_buffer_t *self )
{
    assert( self );

    size_t i;


    for( i=0; i<MAX_VERTEX_ATTRIBUTE; ++i )
    {
        if( self->attributes[i] )
        {
            vertex_attribute_delete( self->attributes[i] );
        }
    }


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
        glDeleteBuffers( 1, &self->indices_id );
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


// ----------------------------------------------------------------------------
const char *
vertex_buffer_format( const vertex_buffer_t *self )
{
    assert( self );

    return self->format;
}


// ----------------------------------------------------------------------------
size_t
vertex_buffer_size( const vertex_buffer_t *self )
{
    assert( self );

    return vector_size( self->items );
}


// ----------------------------------------------------------------------------
void
vertex_buffer_print( vertex_buffer_t * self )
{
    assert(self);

    int i = 0;

    fprintf( stderr, "%ld vertices, %ld indices\n",
             vector_size( self->vertices ), vector_size( self->indices ) );

    while( self->attributes[i] )
    {
        if( self->attributes[i]->target > 0 )
        {
            switch(self->attributes[i]->target )
            {
            case GL_VERTEX_ARRAY:
                fprintf( stderr, " -> Position: ");
                break;
            case GL_NORMAL_ARRAY:
                fprintf( stderr, " -> Normal: ");
                break;
            case GL_COLOR_ARRAY:
                fprintf( stderr, " -> Color: ");
                break;
            case GL_TEXTURE_COORD_ARRAY:
                fprintf( stderr, " -> Texture coord: ");
                break;
            case GL_FOG_COORD_ARRAY:
                fprintf( stderr, " -> Fog coord: ");
                break;
            case GL_SECONDARY_COLOR_ARRAY:
                fprintf( stderr, " -> Secondary color: ");
                break;
            case GL_EDGE_FLAG_ARRAY:
                fprintf( stderr, " -> Edge flag: ");
                break;
            default:
                fprintf( stderr, " -> Unknown: ");
                break;
            }
        }
        else
        {
            fprintf( stderr, " -> Generic attribute n°%d: ",
                     self->attributes[i]->index );
        }
        fprintf(stderr, "%dx%s (+%ld)\n",
                self->attributes[i]->size, 
                GL_TYPE_STRING( self->attributes[i]->type ),
                (long) self->attributes[i]->pointer);

        i += 1;
    }
}


// ----------------------------------------------------------------------------
void
vertex_buffer_upload ( vertex_buffer_t *self )
{
    if( !self->vertices_id )
    {
        glGenBuffers( 1, &self->vertices_id );
        glBindBuffer( GL_ARRAY_BUFFER, self->vertices_id );
        glBufferData( GL_ARRAY_BUFFER,
                      self->vertices->size*self->vertices->item_size,
                      self->vertices->items, GL_DYNAMIC_DRAW );
    }
    if( !self->indices_id )
    {
        glGenBuffers( 1, &self->indices_id );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, self->indices_id );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                      self->indices->size*self->indices->item_size,
                      self->indices->items, GL_DYNAMIC_DRAW );
    }

    glBindBuffer( GL_ARRAY_BUFFER, self->vertices_id );
    glBufferSubData( GL_ARRAY_BUFFER, 0,
                     self->vertices->size*self->vertices->item_size,
                     self->vertices->items );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, self->indices_id );
    glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0,
                     self->indices->size*self->indices->item_size,
                     self->indices->items );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}



// ----------------------------------------------------------------------------
void
vertex_buffer_clear( vertex_buffer_t *self )
{
    assert( self );

    vector_clear( self->indices );
    vector_clear( self->vertices );
    self->dirty = 1;
}



// ----------------------------------------------------------------------------
void
vertex_buffer_render_setup ( vertex_buffer_t *self,
                             GLenum mode, const char *what )
{
    if( self->dirty )
    {
        vertex_buffer_upload( self );
        self->dirty = 0;
    }
    
    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
    glBindBuffer( GL_ARRAY_BUFFER, self->vertices_id );

    size_t i;
    for( i=0; i<MAX_VERTEX_ATTRIBUTE; ++i )
    {
        vertex_attribute_t *attribute = self->attributes[i];
        if ( attribute == 0 )
        {
            break;
        }
        else
        {
            if (attribute->ctarget == 'g')
            {
                (*(attribute->enable))( attribute );
            }
            else if ( strchr(what, attribute->ctarget) )
            {
                (*(attribute->enable))( attribute );
            }
        }
    }
    if( self->indices->size )
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, self->indices_id );
    }
    self->mode = mode;
}

// ----------------------------------------------------------------------------
void
vertex_buffer_render_finish ( vertex_buffer_t *self )
{
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glPopClientAttrib( );
}


// ----------------------------------------------------------------------------
void
vertex_buffer_render_item ( vertex_buffer_t *self,
                            size_t index )
{ 
    assert( self );
    assert( index < vector_size( self->items ) );

    ivec4 * item = (ivec4 *) vector_get( self->items, index );

    if( self->indices->size )
    {
        size_t start = item->istart;
        size_t count = item->icount;
        glDrawElements( self->mode, count, GL_UNSIGNED_INT, (void *)(start*sizeof(GLuint)) );
    }
    else if( self->vertices->size )
    {
        size_t start = item->vstart;
        size_t count = item->vcount;
        glDrawArrays( self->mode, start*self->vertices->item_size, count);
    }
}


// ----------------------------------------------------------------------------
void
vertex_buffer_render ( vertex_buffer_t *self,
                       GLenum mode, const char *what )
{
    size_t vcount = self->vertices->size;
    size_t icount = self->indices->size;

    vertex_buffer_render_setup( self, mode, what );
    if( icount )
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, self->indices_id );
        glDrawElements( mode, icount, GL_UNSIGNED_INT, 0 );
    }
    else
    {
        glDrawArrays( mode, 0, vcount );
    }
    vertex_buffer_render_finish( self );
}
    


// ----------------------------------------------------------------------------
void
vertex_buffer_push_back_indices ( vertex_buffer_t * self,
                                  GLuint * indices,
                                  size_t icount )
{
    assert( self );

    self->dirty = 1;
    vector_push_back_data( self->indices, indices, icount );
}



// ----------------------------------------------------------------------------
void
vertex_buffer_push_back_vertices ( vertex_buffer_t * self,
                                   void * vertices,
                                   size_t vcount )
{
    assert( self );

    self->dirty = 1;
    vector_push_back_data( self->vertices, vertices, vcount );
}



// ----------------------------------------------------------------------------
void
vertex_buffer_insert_indices ( vertex_buffer_t *self,
                               size_t index,
                               GLuint *indices,
                               size_t count )
{
    assert( self );
    assert( self->indices );
    assert( index < self->indices->size+1 );

    self->dirty = 1;
    vector_insert_data( self->indices, index, indices, count );
}



// ----------------------------------------------------------------------------
void
vertex_buffer_insert_vertices( vertex_buffer_t *self,
                               size_t index,
                               void *vertices,
                               size_t vcount )
{
    assert( self );
    assert( self->vertices );
    assert( index < self->vertices->size+1 );

    self->dirty = 1;

    size_t i;
    for( i=0; i<self->indices->size; ++i )
    {
        if( *(GLuint *)(vector_get( self->indices, i )) > index )
        {
            *(GLuint *)(vector_get( self->indices, i )) += index;
        }
    }

    vector_insert_data( self->vertices, index, vertices, vcount );
}



// ----------------------------------------------------------------------------
void
vertex_buffer_erase_indices( vertex_buffer_t *self,
                             size_t first,
                             size_t last )
{
    assert( self );
    assert( self->indices );
    assert( first < self->indices->size );
    assert( (last) <= self->indices->size );

    self->dirty = 1;
    vector_erase_range( self->indices, first, last );
}



// ----------------------------------------------------------------------------
void
vertex_buffer_erase_vertices( vertex_buffer_t *self,
                              size_t first,
                              size_t last )
{
    assert( self );
    assert( self->vertices );
    assert( first < self->vertices->size );
    assert( (first+last) <= self->vertices->size );
    assert( last > first );

    self->dirty = 1;
    size_t i;
    for( i=0; i<self->indices->size; ++i )
    {
        if( *(GLuint *)(vector_get( self->indices, i )) > first )
        {
            *(GLuint *)(vector_get( self->indices, i )) -= (last-first);
        }
    }
    vector_erase_range( self->vertices, first, last );    
}



// ----------------------------------------------------------------------------
size_t
vertex_buffer_push_back( vertex_buffer_t * self,
                         void * vertices, size_t vcount,  
                         GLuint * indices, size_t icount )
{
    return vertex_buffer_insert( self, vector_size( self->items ),
                                 vertices, vcount, indices, icount );
}

// ----------------------------------------------------------------------------
size_t
vertex_buffer_insert( vertex_buffer_t * self, size_t index,
                      void * vertices, size_t vcount,  
                      GLuint * indices, size_t icount )
{
    assert( self );
    assert( vertices );
    assert( indices );

    // Push back vertices
    size_t vstart = vector_size( self->vertices );
    vertex_buffer_push_back_vertices( self, vertices, vcount );

    // Push back indices
    size_t istart = vector_size( self->indices );
    vertex_buffer_push_back_indices( self, indices, icount );

    // Update indices within the vertex buffer
    size_t i;
    for( i=0; i<icount; ++i )
    {
        *(GLuint *)(vector_get( self->indices, istart+i )) += vstart;
    }
    
    // Insert item
    ivec4 item = {{ vstart, vcount, istart, icount }};
    vector_insert( self->items, index, &item );

    return index;
}

// ----------------------------------------------------------------------------
void
vertex_buffer_erase( vertex_buffer_t * self,
                     size_t index )
{
    assert( self );
    assert( index < vector_size( self->items ) );

    ivec4 * item = (ivec4 *) vector_get( self->items, index );
    size_t vstart = item->vstart;
    size_t vcount = item->vcount;
    size_t istart = item->istart;
    size_t icount = item->icount;

    // Update items
    size_t i;
    for( i=0; i<vector_size(self->items); ++i )
    {
        ivec4 * item = (ivec4 *) vector_get( self->items, i );
        if( item->vstart > vstart)
        {
            item->vstart -= vcount;
            item->istart -= icount;
        }
    }
    vertex_buffer_erase_indices( self, istart, istart+icount );
    vertex_buffer_erase_vertices( self, istart, vstart+vcount );
    vector_erase( self->items, index );
}
