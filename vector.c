// ============================================================================
// Freetype GL - A C OpenGL Freetype engine
// Platform:    Any
// API version: 1.0
// WWW:         http://code.google.com/p/freetype-gl/
// ----------------------------------------------------------------------------
// Copyright (c) 2011 Nicolas P. Rougier <Nicolas.Rougier@inria.fr>
// 
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program. If not, see <http://www.gnu.org/licenses/>.
// ============================================================================
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vector.h"

vector *
vector_new( size_t item_size )
{
    assert( item_size );

    vector *self = (vector *) malloc( sizeof(vector) );
    if( !self )
    {
        return NULL;
    }
    self->item_size = item_size;
    self->size      = 0;
    self->capacity  = 1;
    self->items     = malloc( self->item_size * self->capacity );
    return self;
}



void
vector_delete( vector *self )
{
    assert( self );
    free( self->items );
    free( self );
}



void *
vector_get( vector *self,
            size_t index )
{
    assert( self );
    assert( self->size );
    assert( index  < self->size );
    return self->items + index * self->item_size;
}



void *
vector_front( vector *self )
{
    assert( self );
    assert( self->size );
    return vector_get( self, 0 );
}



void *
vector_back( vector *self )
{
    assert( self );
    assert( self->size );
    return vector_get( self, self->size-1 );
}



int
vector_contains( vector *self,
                 void *item,
                 int (*cmp)(void *, void *) )
{
    size_t i;
    assert( self );
    for( i=0; i<self->size; ++i )
    {
        if( (*cmp)(item, vector_get(self,i) ) == 0 )
        {
            return 1;
        }
    }
   return 0;
}



int
vector_empty( vector *self )
{
    assert( self );
    return self->size == 0;
}



size_t
vector_size( vector *self )
{
    assert( self );
    return self->size;
}



void
vector_reserve( vector *self,
                size_t size )
{
    assert( self );
    if( self->capacity < size);
    {
        self->items = realloc( self->items, size * self->item_size );
        self->capacity = size;
    }
}



size_t
vector_capacity( vector *self )
{
    assert( self );
    return self->capacity;
}



void
vector_shrink( vector *self )
{
    assert( self );
    if( self->capacity > self->size )
    {
        self->items = realloc( self->items, self->size * self->item_size );
    }
    self->capacity = self->size;
}



void
vector_clear( vector *self )
{
    assert( self );
    self->size = 0;
}


void
vector_set( vector *self,
            size_t index,
            void *item )
{
    assert( self );
    assert( self->size );
    assert( index  < self->size );
    memcpy( self->items + index * self->item_size,
            item, self->item_size );
}



void
vector_insert( vector *self,
               size_t index,
               void *item )
{
    assert( self );
    assert( index <= self->size);

    if( self->capacity <= self->size )
    {
        vector_reserve(self, 2 * self->capacity );
    }
    if( index < self->size )
    {
        memmove( self->items + (index + 1) * self->item_size,
                 self->items + (index + 0) * self->item_size,
                 (self->size - index)  * self->item_size);
    }
    self->size++;
    vector_set( self, index, item );
}



void
vector_erase_range( vector *self,
                    size_t first,
                    size_t last )
{
    assert( self );
    assert( first < self->size );
    assert( last  < self->size+1 );
    assert( first < last );
    memmove( self->items + first * self->item_size,
             self->items + last  * self->item_size,
             (self->size - last)   * self->item_size);
    self->size -= (last-first);
}



void
vector_erase( vector *self,
              size_t index )
{
    assert( self );
    assert( index < self->size );

    vector_erase_range( self, index, index+1 );
}



void
vector_push_back( vector *self,
                  void *item )
{
    vector_insert( self, self->size, item );
}



void
vector_pop_back( vector *self )
{
    assert( self );
    assert( self->size );
    self->size--;
}



void
vector_resize( vector *self,
               size_t size )
{
    assert( self );
    if( size > self->capacity)
    {
        vector_reserve( self, size );
        self->size = self->capacity;
    }
    else
    {
        self->size = size;
    }
}



void
vector_push_back_data( vector *self,
                       void * data,
                       size_t count )
{
    assert( self );
    assert( data );
    assert( count );

    if( self->capacity < (self->size+count) )
    {
        vector_reserve(self, self->size+count);
    }
    memmove( self->items + self->size * self->item_size, data,
             count*self->item_size );
    self->size += count;
}



void
vector_insert_data( vector *self,
                    size_t index,
                    void * data,
                    size_t count )
{
    assert( self );
    assert( index < self->size );
    assert( data );
    assert( count );

    if( self->capacity < (self->size+count) )
    {
        vector_reserve(self, self->size+count);
    }
    memmove( self->items + (index + count ) * self->item_size,
             self->items + (index ) * self->item_size,
             count*self->item_size );
    memmove( self->items + index * self->item_size, data,
             count*self->item_size );
    self->size += count;
}



void
vector_sort( vector *self,
             int (*cmp)(void *, void *) )
{
    assert( self );
    assert( self->size );
    qsort(self->items, self->size, self->item_size, cmp);
}
