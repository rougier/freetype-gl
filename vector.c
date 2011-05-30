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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vector.h"

vector *
vector_new( size_t itemsize,
            void * ( *item_new    )( void   ),
            void   ( *item_delete )( void * ),
            void * ( *item_copy   )( void * ) )
{
    assert( itemsize );

    vector *self = (vector *) malloc( sizeof(vector) );
    if( !self )
    {
        return NULL;
    }
    self->itemsize    = itemsize;
    self->size        = 0;
    self->capacity    = 1;
    self->items       = malloc( self->itemsize * self->capacity );
    self->item_new    = item_new;
    self->item_delete = item_delete;
    self->item_copy   = item_copy;
    return self;
}

void
vector_delete( vector *self )
{
    assert( self );

    if( self->items )
    {
        free( self->items );
    }
    self->items = 0;
    self->size = 0;
    self->capacity = 0;
    free( self );
}

void *
vector_get( vector *self, size_t index )
{
    assert( self );
    assert( self->size );
    assert( index  < self->size );
    return self->items + index * self->itemsize;
}

void *
vector_front( vector *self )
{
    assert( self );
    assert( self->size );
    return vector_get( self, self->size-1 );
}

void *
vector_back( vector *self )
{
    assert( self );
    assert( self->size );
    return vector_get( self, 0 );
}

int
vector_contains( vector *self, void *item,
                 int (*cmp)(const void *, const void *))
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
vector_reserve( vector *self, size_t size )
{
    assert( self );
    if( self->capacity < size);
    {
        self->items = realloc( self->items, size * self->itemsize );
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
        self->items = realloc( self->items, self->size * self->itemsize );
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
vector_set( vector *self, size_t index, void *item )
{
    assert( self );
    assert( self->size );
    assert( index  < self->size );
    memcpy(self->items + index * self->itemsize,
           item, self->itemsize );
}


void
vector_insert( vector *self, size_t index, void *item )
{
    assert( self );
    assert( index <= self->size);

    if( self->capacity <= self->size )
    {
        vector_reserve(self, 2 * self->capacity );
    }
    if( index < self->size )
    {

        memmove( self->items + (index + 1) * self->itemsize,
                 self->items + (index + 0) * self->itemsize,
                 (self->size - index)  * self->itemsize);
    }
    self->size++;
    vector_set( self, index, item );
}

void
vector_erase_range( vector *self, size_t first, size_t last )
{
    assert( self );
    assert( first < self->size );
    assert( last  < self->size+1 );
    assert( first < last );
    memmove( self->items + first * self->itemsize,
             self->items + last  * self->itemsize,
             (self->size - last)   * self->itemsize);
    self->size -= (last-first);
}

void
vector_erase( vector *self, size_t index )
{
    assert( self );
    assert( index < self->size );

    vector_erase_range( self, index, index+1 );
}

void
vector_push_back( vector *self, void *item )
{
    vector_insert( self, self->size, item );
}

void *
vector_pop_back( vector *self )
{
    assert( self );
    assert( self->size );
    void *item = vector_get(self, self->size-1);
    self->size--;
    return item;
}

void
vector_resize( vector *self, size_t size )
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
vector_add( vector *self, vector * other )
{
    assert( self );
    assert( other );
    assert( self->itemsize == other->itemsize );

    if( self->capacity < (self->size+other->size) )
    {
        vector_reserve(self, self->size+other->size);
    }
    memmove( self->items + self->size * self->itemsize, other->items,
             other->size*other->itemsize );
    self->size += other->size;
}

void
vector_add_data( vector *self, void * data, size_t count )
{
    assert( self );
    assert( data );
    assert( count );

    if( self->capacity < (self->size+count) )
    {
        vector_reserve(self, self->size+count);
    }
    memmove( self->items + self->size * self->itemsize, data,
             count*self->itemsize );
    self->size += count;
}

void
vector_insert_data( vector *self, size_t index, void * data, size_t count )
{
    assert( self );
    assert( index < self->size );
    assert( data );
    assert( count );

    if( self->capacity < (self->size+count) )
    {
        vector_reserve(self, self->size+count);
    }
    memmove( self->items + (index + count ) * self->itemsize,
             self->items + (index ) * self->itemsize,
             count*self->itemsize );
    memmove( self->items + index * self->itemsize, data,
             count*self->itemsize );
    self->size += count;
}

void
vector_merge( vector *self, vector * other,
              int (*cmp)(const void *, const void *))
{
    size_t i;

    assert( self );
    assert( other );
    assert( self->itemsize == other->itemsize );

    for( i=0; i<other->size; ++i )
    {
        void *item = vector_get( other, i );
        if( !vector_contains( self, item, cmp ) )
            {
                vector_push_back( self, item );
            }
    }
}


void
vector_sort( vector *self,
            int (*cmp)(const void *, const void *))
{
    assert( self );
    assert( self->size );

    qsort(self->items, self->size, self->itemsize, cmp);
}
