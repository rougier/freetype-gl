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
#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "texture-atlas.h"

#define max(a,b) (a)>(b)?(a):(b)
#define min(a,b) (a)<(b)?(a):(b)

typedef struct { int x, y, width; } Node;




/* ------------------------------------------------------------------------- */
TextureAtlas *
texture_atlas_new( size_t width, size_t height, size_t depth )
{
    assert( (depth == 1) || (depth == 3) );

    TextureAtlas *self = (TextureAtlas *) malloc( sizeof(TextureAtlas) );
    if( !self )
    {
        return NULL;
    }
    self->nodes = vector_new( sizeof(Node) );
    self->used = 0;
    self->width = width;
    self->height = height;
    self->depth = depth;
    Node node = {0,0,width};
    vector_push_back( self->nodes, &node );
    self->texid = 0;
    self->data = (unsigned char *)
        calloc( width*height*depth, sizeof(unsigned char) );


    // This is a special region that is used for background and underlined
    // decorations of glyphs
    int n = 4;
    unsigned char buffer[n*n];
    memset(buffer, 255, n*n);
    Region r = texture_atlas_get_region( self, n, n );
    texture_atlas_set_region( self, r.x, r.y, r.width, r.height, buffer, 1);
    self->black.x     = r.x + 1;
    self->black.y     = r.y + 1;
    self->black.width = r.width - 2;
    self->black.height= r.height - 2;

    return self;
}



/* ------------------------------------------------------------------------- */
void
texture_atlas_delete( TextureAtlas *self )
{
    assert( self );
    vector_delete( self->nodes );
    if( self->data )
    {
        free( self->data );
    }
    if( self->texid )
    {
        glDeleteTextures( 1, &self->texid );
    }
    free( self );
}



/* ------------------------------------------------------------------------- */
void
texture_atlas_upload( TextureAtlas *self )
{
    assert( self );
    assert( self->data );
    if( !self->texid )
    {
        glGenTextures( 1, &self->texid );
    }
    glBindTexture( GL_TEXTURE_2D, self->texid );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    if( self->depth == 3 )
    {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, self->width, self->height,
                      0, GL_RGB, GL_UNSIGNED_BYTE, self->data );
    }
    else
    {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, self->width, self->height,
                      0, GL_ALPHA, GL_UNSIGNED_BYTE, self->data );
    }
}



/* ------------------------------------------------------------------------- */
void
texture_atlas_set_region( TextureAtlas *self,
                          size_t x, size_t y,
                          size_t width, size_t height,
                          unsigned char *data, size_t stride )
{
    assert( self );
    assert( x < self->width);
    assert( (x + width) <= self->width);
    assert( y < self->height);
    assert( (y + height) <= self->height);
    size_t i;
    size_t depth = self->depth;
    size_t charsize = sizeof(char);
    for( i=0; i<height; ++i )
    {
        memcpy( self->data+((y+i)*self->width + x ) * charsize * depth, 
                data + (i*stride) * charsize, width * charsize * depth  );
    }
}



/* ------------------------------------------------------------------------- */
int
texture_atlas_fit( TextureAtlas *self,
                   size_t index, size_t width, size_t height )
{
    Node *node = (Node *) (vector_get( self->nodes, index ));
    int x = node->x, y, width_left = width;
	size_t i = index;

	if ( (x + width) > self->width )
    {
		return -1;
    }
	y = node->y;
	while( width_left > 0 )
	{
        node = (Node *) (vector_get( self->nodes, i ));
		y = max( y, node->y );
		if( (y + height) > self->height )
        {
			return -1;
        }
		width_left -= node->width;
		++i;
	}
	return y;
}



/* ------------------------------------------------------------------------- */
void
texture_atlas_merge( TextureAtlas *self )
{
    Node *node, *next;
    size_t i;

	for( i=0; i< self->nodes->size-1; ++i )
    {
        node = (Node *) (vector_get( self->nodes, i ));
        next = (Node *) (vector_get( self->nodes, i+1 ));

		if( node->y == next->y )
		{
			node->width += next->width;
            vector_erase( self->nodes, i+1 );
			--i;
		}
    }
}



/* ------------------------------------------------------------------------- */
Region
texture_atlas_get_region( TextureAtlas *self,
                          size_t width, size_t height )
{
    assert( self );
/*
    assert( width );
    assert( height );
*/
	int y, best_height, best_width, best_index;
    Node *node, *prev;
    Region region = {0,0,width,height};
    size_t i;

    best_height = INT_MAX;
    best_index  = -1;
    best_width = INT_MAX;
	for( i=0; i<self->nodes->size; ++i )
	{
        y = texture_atlas_fit( self, i, width, height );
		if( y >= 0 )
		{
            node = (Node *) vector_get( self->nodes, i );
			if( ( y + height < best_height ) ||
                ( y + height == best_height && node->width < best_width) )
			{
				best_height = y + height;
				best_index = i;
				best_width = node->width;
				region.x = node->x;
				region.y = y;
			}
        }
    }
   
	if( best_index == -1 )
    {
        region.x = -1;
        region.y = -1;
        region.width = 0;
        region.height = 0;
        return region;
    }

    node = (Node *) malloc( sizeof(Node) );
    node->x     = region.x;
    node->y     = region.y + height;
    node->width = width;
    vector_insert( self->nodes, best_index, node );
    free( node );

    for(i = best_index+1; i < self->nodes->size; ++i)
    {
        node = (Node *) vector_get( self->nodes, i );
        prev = (Node *) vector_get( self->nodes, i-1 );

        if (node->x < (prev->x + prev->width) )
        {
            int shrink = prev->x + prev->width - node->x;
            node->x     += shrink;
            node->width -= shrink;
            if (node->width <= 0)
            {
                vector_erase( self->nodes, i );
                --i;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    texture_atlas_merge( self );
    self->used += width * height;
    return region;
}
