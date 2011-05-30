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
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <assert.h>
#include <stdlib.h>
#include "texture-glyph.h"



TextureGlyph *
texture_glyph_new( void )
{
    TextureGlyph *self = (TextureGlyph *) malloc( sizeof(TextureGlyph) );
    if(! self )
    {
        return NULL;
    }
    self->size.width  = 0;
    self->size.height = 0;
    self->offset.x    = 0;
    self->offset.y    = 0;
    self->advance.x   = 0.0;
    self->advance.y   = 0.0;
    self->texcoords.s = 0.0;
    self->texcoords.t = 0.0;
    self->texcoords.p = 0.0;
    self->texcoords.q = 0.0;
    return self;
}

void
texture_glyph_delete( TextureGlyph *self )
{
    assert( self );
    free( self );
}

void
texture_glyph_render( TextureGlyph *self,
                      vec2 *pen )
{
    assert( self );

    int x  = pen->x + self->offset.x;
    int y  = pen->y + self->offset.y;
    int w  = self->size.width;
    int h  = self->size.height;

    float u0 = self->texcoords.u0;
    float v0 = self->texcoords.v0;
    float u1 = self->texcoords.u1;
    float v1 = self->texcoords.v1;

    glBegin( GL_TRIANGLES );
    {
        glTexCoord2f( u0, v0 ); glVertex2i( x,   y   );
        glTexCoord2f( u0, v1 ); glVertex2i( x,   y-h );
        glTexCoord2f( u1, v1 ); glVertex2i( x+w, y-h );
        
        glTexCoord2f( u0, v0 ); glVertex2i( x,   y   );
        glTexCoord2f( u1, v1 ); glVertex2i( x+w, y-h );
        glTexCoord2f( u1, v0 ); glVertex2i( x+w, y   );
    }
    glEnd();

    pen->x += self->advance.x;
    pen->y += self->advance.y;
}

void
texture_glyph_add_to_vertex_buffer( TextureGlyph *self,
                                    VertexBuffer *buffer,
                                    vec2 *pen )
{
    int x0  = pen->x + self->offset.x;
    int y0  = pen->y + self->offset.y;
    int x1  = x0 + self->size.width;
    int y1  = y0 - self->size.height;
    float u0 = self->texcoords.u0;
    float v0 = self->texcoords.v0;
    float u1 = self->texcoords.u1;
    float v1 = self->texcoords.v1;

    GLuint index = buffer->vertices->size;
    GLuint indices[] = {index, index+1, index+2,
                        index, index+2, index+3};
    TextureGlyphVertex vertices[] = { { x0,y0,0,  u0,v0,  0,0,0,1 },
                                      { x0,y1,0,  u0,v1,  0,0,0,1 },
                                      { x1,y1,0,  u1,v1,  0,0,0,1 },
                                      { x1,y0,0,  u1,v0,  0,0,0,1 } };
    vertex_buffer_push_back_indices( buffer, indices, 6 );
    vertex_buffer_push_back_vertices( buffer, vertices, 4 );


    pen->x += self->advance.x;
    pen->y += self->advance.y;
}

float 
texture_glyph_get_kerning( TextureGlyph *self,
                           wchar_t charcode )
{
    size_t i;

    assert( self );
    if( !self->kerning )
    {
        return 0;
    }

    for( i=0; i<self->kerning_count; ++i )
    {
        if( self->kerning[i].charcode == charcode )
        {
            return self->kerning[i].kerning;
        }
    }
    return 0;
}
