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
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vertex-attribute.h"



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
        attribute->enable = vertex_attribute_position_enable;
        break;

    case GL_NORMAL_ARRAY:
        assert (size == 3);
        assert( (type == GL_BYTE) || (type == GL_SHORT) ||
                (type == GL_INT)  || (type == GL_FLOAT) ||
                (type == GL_DOUBLE) );
        attribute->enable = vertex_attribute_normal_enable;
        break;

    case GL_COLOR_ARRAY:
        assert( (size == 3) || (size == 4) );
        assert( (type == GL_BYTE)  || (type == GL_UNSIGNED_BYTE)  ||
                (type == GL_SHORT) || (type == GL_UNSIGNED_SHORT) ||
                (type == GL_INT)   || (type == GL_UNSIGNED_INT)   ||
                (type == GL_FLOAT) || (type == GL_DOUBLE) );
        attribute->enable = vertex_attribute_color_enable;
        break;

    case GL_TEXTURE_COORD_ARRAY:
        assert( (type == GL_SHORT) || (type == GL_INT) ||
                (type == GL_FLOAT) || (type == GL_DOUBLE) );
        attribute->enable = vertex_attribute_tex_coord_enable;
        break;

    case GL_FOG_COORD_ARRAY:
        assert( size == 2 );
        assert( (type == GL_FLOAT) || (type == GL_DOUBLE) );
        attribute->enable = vertex_attribute_fog_coord_enable;
        break;


    case GL_EDGE_FLAG_ARRAY:
        assert( size == 1 );
        assert( type == GL_BOOL );
        attribute->enable = vertex_attribute_edge_flag_enable;
        break;

    case GL_SECONDARY_COLOR_ARRAY:
        assert( size == 3 );
        assert( (type == GL_BYTE)  || (type == GL_UNSIGNED_BYTE)  ||
                (type == GL_SHORT) || (type == GL_UNSIGNED_SHORT) ||
                (type == GL_INT)   ||  (type == GL_UNSIGNED_INT)  ||
                (type == GL_FLOAT) || (type == GL_DOUBLE) );
        attribute->enable = vertex_attribute_secondary_color_enable;
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
    switch(ctype)
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
