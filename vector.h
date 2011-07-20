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
   ========================================================================= */
#ifndef __VECTOR_H__
#define __VECTOR_H__



/**
 *  Generic vector structure.
 */
typedef struct
 {
     /** Pointer to dynamically allocated items. */
     void * items;

     /** Number of items that can be held in currently allocated storage. */
     size_t capacity;

     /** Number of items. */
     size_t size;

     /** Size (in bytes) of a single item. */
     size_t item_size;
} Vector;

/**
 *  Creates a vector.
 *
 *  @param  item_size    item size in bytes
 *  @return              a new empty vector
 */
  Vector *
  vector_new( size_t item_size );

/**
 *  Deletes a vector.
 *
 *  @param  self a vector structure
 */
  void
  vector_delete( Vector *self );

/**
 *  Returns a pointer to the item located at specified index.
 *
 *  @param  self  a vector structure
 *  @param  index the index of the item to be returned
 *  @return       pointer on the specified item
 */
  const void *
  vector_get( const Vector *self,
              size_t index );

/**
 *  Returns a pointer to the first item.
 *
 *  @param  self  a vector structure
 *  @return       pointer on the first item
 */
  const void *
  vector_front( const Vector *self );

/**
 *  Returns a pointer to the last item
 *
 *  @param  self  a vector structure
 *  @return pointer on the last item
 */
  const void *
  vector_back( const Vector *self );

/**
 *  Check if an item is contained within the vector.
 *
 *  @param  self  a vector structure
 *  @param  item  item to be searched in the vector
 *  @param  cmp   a pointer a comparison function
 *  @return       1 if item is contained within the vector, 0 otherwise
 */
  int
  vector_contains( const Vector *self,
                   const void *item,
                   int (*cmp)(const void *, const void *) );

/**
 *  Checks whether the vector is empty.
 *
 *  @param  self  a vector structure
 *  @return       1 if the vector is empty, 0 otherwise
 */
  int
  vector_empty( const Vector *self );

/**
 *  Returns the number of items
 *
 *  @param  self  a vector structure
 *  @return       number of items
 */
  size_t
  vector_size( const Vector *self );

/**
 *  Reserve storage such that it can hold at last size items.
 *
 *  @param  self  a vector structure
 *  @param  size  the new storage capacity
 */
  void
  vector_reserve( Vector *self,
                  const size_t size );

/**
 *  Returns current storage capacity
 *
 *  @param  self  a vector structure
 *  @return       storage capacity
 */
  size_t
  vector_capacity( const Vector *self );

/**
 *  Decrease capacity to fit actual size.
 *
 *  @param  self  a vector structure
 */
  void
  vector_shrink( Vector *self );

/**
 *  Removes all items.
 *
 *  @param  self  a vector structure
 */
  void
  vector_clear( Vector *self );

/**
 *  Replace an item.
 *
 *  @param  self  a vector structure
 *  @param  index the index of the item to be replaced
 *  @param  item  the new item
 */
  void
  vector_set( Vector *self,
              const size_t index,
              const void *item );

/**
 *  Erase an item.
 *
 *  @param  self  a vector structure
 *  @param  index the index of the item to be erased
 */
  void
  vector_erase( Vector *self,
                const size_t index );

/**
 *  Erase a range of items.
 *
 *  @param  self  a vector structure
 *  @param  first the index of the first item to be erased
 *  @param  last  the index of the last item to be erased
 */
  void
  vector_erase_range( Vector *self,
                      const size_t first,
                      const size_t last );

/**
 *  Appends given item to the end of the vector.
 *
 *  @param  self a vector structure
 *  @param  item the item to be inserted
 */
  void
  vector_push_back( Vector *self,
                    const void *item );

/**
 *  Removes the last item of the vector.
 *
 *  @param  self a vector structure
 */
  void
  vector_pop_back( Vector *self );

/**
 *  Resizes the vector to contain size items
 *
 *  If the current size is less than size, additional items are appended and
 *  initialized with value. If the current size is greater than size, the
 *  vector is reduced to its first size elements.
 *
 *  @param  self a vector structure
 *  @param  size the new size
 */
  void
  vector_resize( Vector *self,
                 const size_t size );

/**
 *  Insert a single item at specified index.
 *
 *  @param  self  a vector structure
 *  @param  index location before which to insert item
 *  @param  item  the item to be inserted
 */
  void
  vector_insert( Vector *self,
                 const size_t index,
                 const void *item );

/**
 *  Insert raw data at specified index.
 *
 *  @param  self  a vector structure
 *  @param  index location before which to insert item
 *  @param  data  a pointer to the items to be inserted
 *  @param  count the number of items to be inserted
 */
  void
  vector_insert_data( Vector *self,
                      const size_t index,
                      const void * data,
                      const size_t count );

/**
 *  Append raw data to the end of the vector.
 *
 *  @param  self  a vector structure
 *  @param  data  a pointer to the items to be inserted
 *  @param  count the number of items to be inserted
 */
  void
  vector_push_back_data( Vector *self,
                         const void * data, 
                         const size_t count );

/**
 *  Sort vector items according to cmp function.
 *
 *  @param  self  a vector structure
 *  @param  cmp   a pointer a comparison function
 */
  void
  vector_sort( Vector *self,
               int (*cmp)(const void *, const void *) );

#endif /* __VECTOR_H__ */
