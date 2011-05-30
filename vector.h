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
#ifndef __VECTOR_H__
#define __VECTOR_H__

/* Structure */
struct __vector__ {
    void * items;
    size_t itemsize;
    size_t capacity;
    size_t size;
    void * (* item_new    )( void   );
    void   (* item_delete )( void * );
    void * (* item_copy   )( void * );
    int    (* item_cmp    )( void *, void * );
};
typedef struct __vector__ vector;



/* General */
vector * vector_new( size_t itemsize,
                     void * ( *item_new    )( void   ),
                     void   ( *item_delete )( void * ),
                     void * ( *copy        )( void * ) );
void     vector_delete( vector *self );

/* Item access */
void *   vector_get( vector *self,
                     size_t index );
void *   vector_front( vector *self );
void *   vector_back( vector *self );
int      vector_contains( vector *self,
                          void *item,
                          int (*cmp)(const void *, const void *));

/* Capacity */
int      vector_empty( vector *self );
size_t   vector_size( vector *self );
void     vector_reserve( vector *self,
                         size_t size );
size_t   vector_capacity( vector *self );
void     vector_shrink( vector *self );

/* Modifiers */
void     vector_clear( vector *self );
void     vector_set( vector *self,
                     size_t index,
                     void *item );
void     vector_erase( vector *self,
                       size_t index );
void     vector_erase_range( vector *self,
                             size_t first,
                             size_t last );
void     vector_push_back( vector *self,
                           void *item );
void *   vector_pop_back( vector *self );
void     vector_resize( vector *self,
                        size_t size );
void     vector_insert( vector *self,
                        size_t index,
                        void *item );
void     vector_insert_data( vector *self,
                             size_t index,
                             void * data,
                             size_t count );
void     vector_add( vector *self,
                     vector * other );
void     vector_add_data( vector *self,
                          void * data, 
                          size_t count );
void     vector_merge( vector *self,
                       vector * other,
                       int (*cmp)(const void *, const void *));
void     vector_sort( vector *self,
                      int (*cmp)(const void *, const void *));

#endif /* __VECTOR_H__ */
