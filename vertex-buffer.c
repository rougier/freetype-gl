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
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vertex-buffer.h"



// ----------------------------------------------------------------------------
vertex_buffer_t *
vertex_buffer_new( char *format )
{
    size_t i, index = 0, stride = 0;
    char *start = 0, *end = 0;
    GLvoid *pointer = 0;

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
            desc = strdup(start);
        }
        else
        {
            desc = strndup(start, end-start);
        }
        vertex_attribute_t *attribute = vertex_attribute_parse( desc );
        start = end+1;
        free(desc);
        attribute->pointer = pointer;
        stride += attribute->size*GL_TYPE_SIZE( attribute->type );
        pointer+= attribute->size*GL_TYPE_SIZE( attribute->type );
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
    self->dirty = 1;
    return self;
}



// ----------------------------------------------------------------------------
vertex_buffer_t *
vertex_buffer_new_from_data( char *format,
                             size_t vcount,
                             void *vertices,
                             size_t icount,
                             GLuint *indices )
{
    vertex_buffer_t *self = vertex_buffer_new( format );

    vector_resize( self->vertices, vcount );
    assert( self->vertices->size == vcount);
    memcpy( self->vertices->items, vertices, vcount*self->vertices->item_size );
    vector_resize( self->indices, icount );
    assert( self->indices->size == icount);
    memcpy( self->indices->items, indices, icount*self->indices->item_size );
    self->dirty = 1;
    return self;
}



// ----------------------------------------------------------------------------
void
vertex_buffer_delete( vertex_buffer_t *self )
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
    }
    if( !self->indices_id )
    {
        glGenBuffers( 1, &self->indices_id );
    }
    glBindBuffer( GL_ARRAY_BUFFER, self->vertices_id );
    glBufferData( GL_ARRAY_BUFFER,
                  self->vertices->size*self->vertices->item_size,
                  self->vertices->items, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, self->indices_id );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                  self->indices->size*self->indices->item_size,
                  self->indices->items, GL_DYNAMIC_DRAW );
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
vertex_buffer_render ( vertex_buffer_t *self,
                       GLenum mode,
                       char *what )
{ 
    assert( self );

    if( !self->vertices->size )
    {
        return;
    }

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
        glDrawElements( mode, self->indices->size, GL_UNSIGNED_INT, 0 );
    }
    else
    {
        glDrawArrays( mode, 0, self->vertices->size );
    }

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glPopClientAttrib( );
}



// ----------------------------------------------------------------------------
void
vertex_buffer_push_back_index ( vertex_buffer_t *self,
                                GLuint index )
{
    assert( self );

    self->dirty = 1;
    vector_push_back( self->indices, &index );
}



// ----------------------------------------------------------------------------
void
vertex_buffer_push_back_vertex ( vertex_buffer_t *self,
                                 void *vertex )
{
    assert( self );

    self->dirty = 1;
    vector_push_back( self->vertices, vertex );
}



// ----------------------------------------------------------------------------
void
vertex_buffer_push_back_indices ( vertex_buffer_t *self,
                                  GLuint *indices,
                                  size_t count )
{
    assert( self );

    self->dirty = 1;
    vector_push_back_data( self->indices, indices, count );
}



// ----------------------------------------------------------------------------
void
vertex_buffer_push_back_vertices ( vertex_buffer_t *self,
                                   void *vertices,
                                   size_t count )
{
    assert( self );

    self->dirty = 1;
    vector_push_back_data( self->vertices, vertices, count );

}



// ----------------------------------------------------------------------------
void
vertex_buffer_insert_indices ( vertex_buffer_t *self,
                               size_t index,
                               GLuint *indices,
                               size_t count )
{
    assert( self );

    self->dirty = 1;
    vector_insert_data( self->indices, index, indices, count );
}



// ----------------------------------------------------------------------------
void
vertex_buffer_insert_vertices ( vertex_buffer_t *self,
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



// ----------------------------------------------------------------------------
void
vertex_attribute_position_enable( vertex_attribute_t *attr )
{
    glEnableClientState( attr->target );
    glVertexPointer( attr->size, attr->type, attr->stride, attr->pointer );
}



// ----------------------------------------------------------------------------
void
vertex_attribute_normal_enable( vertex_attribute_t *attr )
{
    glEnableClientState( attr->target );
    glNormalPointer( attr->type, attr->stride, attr->pointer );
}



// ----------------------------------------------------------------------------
void
vertex_attribute_color_enable( vertex_attribute_t *attr )
{
    glEnableClientState( attr->target );
    glColorPointer( attr->size, attr->type, attr->stride, attr->pointer );
}



// ----------------------------------------------------------------------------
void
vertex_attribute_tex_coord_enable( vertex_attribute_t *attr )
{
    glEnableClientState( attr->target );
    glTexCoordPointer( attr->size, attr->type, attr->stride, attr->pointer );
}



// ----------------------------------------------------------------------------
void
vertex_attribute_fog_coord_enable( vertex_attribute_t *attr )
{
    glEnableClientState( attr->target );
    glFogCoordPointer( attr->type, attr->stride, attr->pointer );
}



// ----------------------------------------------------------------------------
void
vertex_attribute_edge_flag_enable( vertex_attribute_t *attr )
{
    glEnableClientState( attr->target );
    glEdgeFlagPointer( attr->stride, attr->pointer );
}



// ----------------------------------------------------------------------------
void
vertex_attribute_secondary_color_enable( vertex_attribute_t *attr )
{
    glEnableClientState( attr->target );
    glSecondaryColorPointer( attr->size, attr->type, attr->stride, attr->pointer );
}



// ----------------------------------------------------------------------------
void
vertex_attribute_generic_enable( vertex_attribute_t *attr )
{
    glEnableVertexAttribArray( attr->index );
    glVertexAttribPointer( attr->index, attr->size, attr->type,
                           attr->normalized, attr->stride, attr->pointer );
}



// ----------------------------------------------------------------------------
vertex_attribute_t *
vertex_attribute_parse( char *format )
{
    // Generic attribute
    char *p = strpbrk ( format, "0123456789" );
    if (p == format) 
    {
        // Normalized
        p = strpbrk ( format, "n" );
        if ( p != NULL )
        {
            int size, index;
            char ctype;
            sscanf( format, "%dgn%d%c", &index, &size, &ctype );
            GLenum type = GL_TYPE( ctype );
            return vertex_attribute_new( 0, index, size, type, GL_TRUE, 0, 0 );
        }
        else
        {
            int size, index;
            char ctype;
            sscanf( format, "%dg%d%c", &index, &size, &ctype );
            GLenum type = GL_TYPE( ctype );
            return vertex_attribute_new( 0, index, size, type, GL_FALSE, 0, 0 );
        }
    }
    
    // Known attribute
    p = strpbrk ( format, "vcntfse" );
    if ( p != 0 )
    {
        int size;
        char ctarget, ctype;
        sscanf( format, "%c%d%c", &ctarget, &size, &ctype );
        GLenum type = GL_TYPE( ctype );
        GLenum target = GL_VERTEX_ATTRIBUTE_TARGET( ctarget );
        return vertex_attribute_new( target, 0, size, type, GL_FALSE, 0, 0 );
    }

    fprintf(stderr, "Vertex attribute format not understood\n");
    return 0;
}



// ----------------------------------------------------------------------------
vertex_attribute_t *
vertex_attribute_new( GLenum target,
                      GLuint index,
                      GLint size,
                      GLenum type,
                      GLboolean normalized,
                      GLsizei stride,
                      GLvoid *pointer )
{
    vertex_attribute_t *attribute = (vertex_attribute_t *) malloc (sizeof(vertex_attribute_t));
    assert( size > 0 );

    // Generic attribute
    if (target == 0)
    {
        attribute->ctarget = 'g';
        assert( (size < 4) );
        assert( (type == GL_BYTE)  || (type == GL_UNSIGNED_BYTE)  ||
                (type == GL_SHORT) || (type == GL_UNSIGNED_SHORT) ||
                (type == GL_INT)   || (type == GL_UNSIGNED_INT)   ||
                (type == GL_FLOAT) || (type == GL_DOUBLE) );
        attribute->enable =
            (void(*)(void *)) vertex_attribute_generic_enable;
    }
    else
    {
        // Standard attribute
        switch( target )
        {
        case GL_VERTEX_ARRAY:
            attribute->ctarget = 'v';
            assert( size > 1 );
            attribute->enable =
                (void(*)(void *)) vertex_attribute_position_enable;
            break;

        case GL_NORMAL_ARRAY:
            attribute->ctarget = 'n';
            assert (size == 3);
            assert( (type == GL_BYTE) || (type == GL_SHORT) ||
                    (type == GL_INT)  || (type == GL_FLOAT) ||
                    (type == GL_DOUBLE) );
            attribute->enable =
                (void(*)(void *)) vertex_attribute_normal_enable;
            break;
            
        case GL_COLOR_ARRAY:
            attribute->ctarget = 'c';
            assert( (size == 3) || (size == 4) );
            assert( (type == GL_BYTE)  || (type == GL_UNSIGNED_BYTE)  ||
                    (type == GL_SHORT) || (type == GL_UNSIGNED_SHORT) ||
                    (type == GL_INT)   || (type == GL_UNSIGNED_INT)   ||
                    (type == GL_FLOAT) || (type == GL_DOUBLE) );
            attribute->enable =
                (void(*)(void *)) vertex_attribute_color_enable;
            break;
        
        case GL_TEXTURE_COORD_ARRAY:
            attribute->ctarget = 't';
            assert( (type == GL_SHORT) || (type == GL_INT) ||
                    (type == GL_FLOAT) || (type == GL_DOUBLE) );
            attribute->enable =
                (void(*)(void *)) vertex_attribute_tex_coord_enable;
            break;

        case GL_FOG_COORD_ARRAY:
            attribute->ctarget = 'f';
            assert( size == 2 );
            assert( (type == GL_FLOAT) || (type == GL_DOUBLE) );
            attribute->enable =
                (void(*)(void *)) vertex_attribute_fog_coord_enable;
            break;
            
        case GL_EDGE_FLAG_ARRAY:
            attribute->ctarget = 'e';
            assert( size == 1 );
            assert( type == GL_BOOL );
            attribute->enable =
                (void(*)(void *)) vertex_attribute_edge_flag_enable;
            break;

        case GL_SECONDARY_COLOR_ARRAY:
            attribute->ctarget = 's';
            assert( size == 3 );
            assert( (type == GL_BYTE)  || (type == GL_UNSIGNED_BYTE)  ||
                    (type == GL_SHORT) || (type == GL_UNSIGNED_SHORT) ||
                    (type == GL_INT)   ||  (type == GL_UNSIGNED_INT)  ||
                (type == GL_FLOAT) || (type == GL_DOUBLE) );
            attribute->enable =
                (void(*)(void *)) vertex_attribute_secondary_color_enable;
            break;

        default:
            fprintf(stderr, "Vertex attribute format not understood\n");
            attribute->enable = 0;
            break;
        }
    }

    attribute->target     = target;
    attribute->index      = index;
    attribute->size       = size;
    attribute->type       = type;
    attribute->normalized = normalized;
    attribute->stride     = stride;
    attribute->pointer    = pointer;

    return attribute;
}



// ----------------------------------------------------------------------------
GLenum
GL_TYPE( char ctype )
{
    switch( ctype )
    {
    case 'b': return GL_BYTE;
    case 'B': return GL_UNSIGNED_BYTE;
    case 's': return GL_SHORT;
    case 'S': return GL_UNSIGNED_SHORT;
    case 'i': return GL_INT;
    case 'I': return GL_UNSIGNED_INT;
    case 'f': return GL_FLOAT;
    case 'd': return GL_DOUBLE;
    default:  return 0;
    }
}



// ----------------------------------------------------------------------------
GLenum
GL_VERTEX_ATTRIBUTE_TARGET( char ctarget )
{
    switch( ctarget )
    {
    case 'v': return GL_VERTEX_ARRAY;
    case 'n': return GL_NORMAL_ARRAY;
    case 'c': return GL_COLOR_ARRAY;
    case 't': return GL_TEXTURE_COORD_ARRAY;
    case 'f': return GL_FOG_COORD_ARRAY;
    case 's': return GL_SECONDARY_COLOR_ARRAY;
    case 'e': return GL_EDGE_FLAG_ARRAY;
    default:  return 0;
    }
}



// ----------------------------------------------------------------------------
GLuint 
GL_TYPE_SIZE( GLenum gtype )
{
    switch( gtype )
    {
    case GL_BOOL:           return sizeof(GLboolean);
    case GL_BYTE:           return sizeof(GLbyte);
    case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
    case GL_SHORT:          return sizeof(GLshort);
    case GL_UNSIGNED_SHORT: return sizeof(GLushort);
    case GL_INT:            return sizeof(GLint);
    case GL_UNSIGNED_INT:   return sizeof(GLuint);
    case GL_FLOAT:          return sizeof(GLfloat);
    case GL_DOUBLE:         return sizeof(GLdouble);
    default:                return 0;
    }
}



char *
GL_TYPE_STRING( GLenum gtype )
{
    switch( gtype )
    {
    case GL_BOOL:           return "GL_BOOL";
    case GL_BYTE:           return "GL_BYTE";
    case GL_UNSIGNED_BYTE:  return "GL_UNSIGNED_BYTE";
    case GL_SHORT:          return "GL_SHORT";
    case GL_UNSIGNED_SHORT: return "GL_UNSIGNED_SHORT";
    case GL_INT:            return "GL_INT";
    case GL_UNSIGNED_INT:   return "GL_UNSIGNED_INT";
    case GL_FLOAT:          return "GL_FLOAT";
    case GL_DOUBLE:         return "GL_DOUBLE";
    default:                return "GL_VOID";
    }
}
