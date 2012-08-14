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
#include "vertex-attribute.h"



// ----------------------------------------------------------------------------
vertex_attribute_t *
vertex_attribute_new( GLchar * name,
                      GLint size,
                      GLenum type,
                      GLboolean normalized,
                      GLsizei stride,
                      GLvoid *pointer )
{
    assert( size > 0 );

    vertex_attribute_t *attribute =
        (vertex_attribute_t *) malloc (sizeof(vertex_attribute_t));

    attribute->name       = (GLchar *) strdup( name );
    attribute->index      = -1;
    attribute->size       = size;
    attribute->type       = type;
    attribute->normalized = normalized;
    attribute->stride     = stride;
    attribute->pointer    = pointer;
    return attribute;
}



// ----------------------------------------------------------------------------
void
vertex_attribute_delete( vertex_attribute_t * self )
{
    assert( self );

    free( self->name );
    free( self );
}



// ----------------------------------------------------------------------------
vertex_attribute_t *
vertex_attribute_parse( char *format )
{
    int size;
    int normalized = 0;
    char ctype;
    char *name;
    char *p = strchr(format, ':');
    if( p != NULL)
    {
        name = strndup(format, p-format);
        if( *(++p) == '\0' ) 
        {
            fprintf( stderr, "No size specified for '%s' attribute\n", name );
            free( name );
            return 0;
        }
        size = *p - '0';

        if( *(++p) == '\0' ) 
        {
            fprintf( stderr, "No format specified for '%s' attribute\n", name );
            free( name );
            return 0;
        }
        ctype = *p;

        if( *(++p) != '\0' ) 
        {
            if( *p == 'n' )
            {
                normalized = 1;
            }
        }

    }
    else
    {
        fprintf(stderr, "Vertex attribute format not understood ('%s')\n", format );   
        return 0;
    }

    GLenum type = GL_TYPE( ctype );
    vertex_attribute_t *attr = vertex_attribute_new( name, size, type, normalized, 0, 0 );
    free( name );
    return attr;
}



// ----------------------------------------------------------------------------
void
vertex_attribute_enable( vertex_attribute_t *attr )
{
    if( attr->index == -1 )
    {
        GLint program;
        glGetIntegerv( GL_CURRENT_PROGRAM, &program );
        if( program == 0)
        {
            return;
        }
        attr->index = glGetAttribLocation( program, attr->name );
        if( attr->index == -1 )
        {
            return;
        }
    }
    glEnableVertexAttribArray( attr->index );
    glVertexAttribPointer( attr->index, attr->size, attr->type,
                           attr->normalized, attr->stride, attr->pointer );
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
    default:                return 0;
    }
}



// ----------------------------------------------------------------------------
const char *
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
    default:                return "GL_VOID";
    }
}
