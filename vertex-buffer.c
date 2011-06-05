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
 * ========================================================================= */
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vertex-buffer.h"


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
                             size_t vcount,
                             void *vertices,
                             size_t icount,
                             GLuint *indices )
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
vertex_buffer_render ( VertexBuffer *self,
                       GLenum mode,
                       char *what )
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
    assert( self );

    self->dirty = 1;
    vector_push_back( self->indices, &index );
}

void
vertex_buffer_push_back_vertex ( VertexBuffer *self,
                                 void *vertex )
{
    assert( self );

    self->dirty = 1;
    vector_push_back( self->vertices, vertex );
}

void
vertex_buffer_push_back_indices ( VertexBuffer *self,
                                  GLuint *indices,
                                  size_t count )
{
    assert( self );

    self->dirty = 1;
    vector_push_back_data( self->indices, indices, count );
}

void
vertex_buffer_push_back_vertices ( VertexBuffer *self,
                                   void *vertices,
                                   size_t count )
{
    assert( self );

    self->dirty = 1;
    vector_push_back_data( self->vertices, vertices, count );

}

void
vertex_buffer_insert_indices ( VertexBuffer *self,
                               size_t index,
                               GLuint *indices,
                               size_t count )
{
    assert( self );

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

void
vertex_attribute_position_enable( VertexAttribute *attr )
{
    glEnableClientState( attr->target );
    glVertexPointer( attr->size, attr->type, attr->stride, attr->pointer );
}



void
vertex_attribute_normal_enable( VertexAttribute *attr )
{
    glEnableClientState( attr->target );
    glNormalPointer( attr->type, attr->stride, attr->pointer );
}



void
vertex_attribute_color_enable( VertexAttribute *attr )
{
    glEnableClientState( attr->target );
    glColorPointer( attr->size, attr->type, attr->stride, attr->pointer );
}



void
vertex_attribute_tex_coord_enable( VertexAttribute *attr )
{
    glEnableClientState( attr->target );
    glTexCoordPointer( attr->size, attr->type, attr->stride, attr->pointer );
}



void
vertex_attribute_fog_coord_enable( VertexAttribute *attr )
{
    glEnableClientState( attr->target );
    glFogCoordPointer( attr->type, attr->stride, attr->pointer );
}



void
vertex_attribute_edge_flag_enable( VertexAttribute *attr )
{
    glEnableClientState( attr->target );
    glEdgeFlagPointer( attr->stride, attr->pointer );
}



void
vertex_attribute_secondary_color_enable( VertexAttribute *attr )
{
    glEnableClientState( attr->target );
    glSecondaryColorPointer( attr->size, attr->type, attr->stride, attr->pointer );
}



VertexAttribute *
vertex_attribute_parse( char *format )
{
    char *p = strpbrk ( format, "vcntfse" );
    if ( p != 0 )
    {
        int size;
        char ctarget, ctype;
        sscanf( p, "%c%d%c", &ctarget, &size, &ctype );
        GLenum type = GL_TYPE( ctype );
        GLenum target = GL_VERTEX_ATTRIBUTE_TARGET( ctarget );
        return vertex_attribute_new( target, size, type, 0, 0 );
    }
    else
    {
    }
    return 0;
}



VertexAttribute *
vertex_attribute_new( GLenum target,
                      GLint size,
                      GLenum type,
                      GLsizei stride,
                      GLvoid *pointer )
{
    VertexAttribute *attribute = (VertexAttribute *) malloc (sizeof(VertexAttribute));

    switch( target )
    {
    case GL_VERTEX_ARRAY:
        assert( size > 1 );
        attribute->enable =
            (void(*)(void *)) vertex_attribute_position_enable;
        break;

    case GL_NORMAL_ARRAY:
        assert (size == 3);
        assert( (type == GL_BYTE) || (type == GL_SHORT) ||
                (type == GL_INT)  || (type == GL_FLOAT) ||
                (type == GL_DOUBLE) );
        attribute->enable =
            (void(*)(void *)) vertex_attribute_normal_enable;
        break;

    case GL_COLOR_ARRAY:
        assert( (size == 3) || (size == 4) );
        assert( (type == GL_BYTE)  || (type == GL_UNSIGNED_BYTE)  ||
                (type == GL_SHORT) || (type == GL_UNSIGNED_SHORT) ||
                (type == GL_INT)   || (type == GL_UNSIGNED_INT)   ||
                (type == GL_FLOAT) || (type == GL_DOUBLE) );
        attribute->enable =
            (void(*)(void *)) vertex_attribute_color_enable;
        break;

    case GL_TEXTURE_COORD_ARRAY:
        assert( (type == GL_SHORT) || (type == GL_INT) ||
                (type == GL_FLOAT) || (type == GL_DOUBLE) );
        attribute->enable =
            (void(*)(void *)) vertex_attribute_tex_coord_enable;
        break;

    case GL_FOG_COORD_ARRAY:
        assert( size == 2 );
        assert( (type == GL_FLOAT) || (type == GL_DOUBLE) );
        attribute->enable =
            (void(*)(void *)) vertex_attribute_fog_coord_enable;
        break;


    case GL_EDGE_FLAG_ARRAY:
        assert( size == 1 );
        assert( type == GL_BOOL );
        attribute->enable =
            (void(*)(void *)) vertex_attribute_edge_flag_enable;
        break;

    case GL_SECONDARY_COLOR_ARRAY:
        assert( size == 3 );
        assert( (type == GL_BYTE)  || (type == GL_UNSIGNED_BYTE)  ||
                (type == GL_SHORT) || (type == GL_UNSIGNED_SHORT) ||
                (type == GL_INT)   ||  (type == GL_UNSIGNED_INT)  ||
                (type == GL_FLOAT) || (type == GL_DOUBLE) );
        attribute->enable =
            (void(*)(void *)) vertex_attribute_secondary_color_enable;
        break;

    default:
        attribute->enable = 0;
        break;
    }

    attribute->target  = target;
    attribute->size    = size;
    attribute->type    = type;
    attribute->stride  = stride;
    attribute->pointer = pointer;

    return attribute;
}


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
