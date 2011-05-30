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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "texture-atlas.h"

#define max(a,b) (a)>(b)?(a):(b)
#define min(a,b) (a)<(b)?(a):(b)

typedef struct { int x, y, width; } Node;



/* ========================================================================= */
/*                                                                           */
/* ========================================================================= */
TextureAtlas *
texture_atlas_new( size_t width, size_t height )
{
    TextureAtlas *self = (TextureAtlas *) malloc( sizeof(TextureAtlas) );
    if( !self )
    {
        return NULL;
    }
    self->nodes = vector_new( sizeof(Node), 0, 0, 0 );
    self->used = 0;
    self->width = width;
    self->height = height;
    Node node = {0,0,width};
    vector_push_back( self->nodes, &node );
    self->texid = 0;
    self->data = (unsigned char *) calloc( width*height, sizeof(unsigned char) );
    return self;
}



/* ========================================================================= */
/*                                                                           */
/* ========================================================================= */
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



/* ========================================================================= */
/*                                                                           */
/* ========================================================================= */
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
    glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, self->width, self->height,
                  0, GL_ALPHA, GL_UNSIGNED_BYTE, self->data );
}



/* ========================================================================= */
/*                                                                           */
/* ========================================================================= */
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
    size_t charsize = sizeof(char);
    for( i=0; i<height; ++i )
    {
        memcpy( self->data+((y+i)*self->width+x) * charsize,
                data+(i*stride) * charsize, width  * charsize );
    }
}



/* ========================================================================= */
/*                                                                           */
/* ========================================================================= */
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



/* ========================================================================= */
/*                                                                           */
/* ========================================================================= */
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



/* ========================================================================= */
/*                                                                           */
/* ========================================================================= */
ivec2
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
    ivec2 pos;
    size_t i;

    best_height = INT_MAX;
    best_index  = -1;
    best_width = INT_MAX;
    pos.x = pos.y = 0;
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
				pos.x = node->x;
				pos.y = y;
			}
        }
    }
   
	if( best_index == -1 )
    {
        pos.x = -1;
        pos.y = -1;
        return pos;
    }

    node = (Node *) malloc( sizeof(Node) );
    node->x     = pos.x;
    node->y     = pos.y + height;
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
    return pos;
}
