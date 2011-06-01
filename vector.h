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
#ifndef __VECTOR_H__
#define __VECTOR_H__

/**
 *  @defgroup vector vector
 *  @{
 */


/**
 *  Generic vector structure
 */
typedef struct {
    /// pointer to dynamically allocated items
    void * items;

    /// number of items that can be held in currently allocated storage
    size_t capacity;

    /// number of items
    size_t size;

    /// size (in bytes) of a single item
    size_t item_size;
} vector;

/**
 *  Creates a vector.
 *
 *  @param  item_size    item size in bytes
 *  @return              a new empty vector
 */
  vector *
  vector_new( size_t item_size );

/**
 *  Deletes a vector.
 *
 *  @param  self a vector structure
 */
  void
  vector_delete( vector *self );

/**
 *  Returns a pointer to the item located at specified index.
 *
 *  @param  self  a vector structure
 *  @param  index the index of the item to be returned
 *  @return       pointer on the specified item
 */
  void *
  vector_get( vector *self,
              size_t index );

/**
 *  Returns a pointer to the first item.
 *
 *  @param  self  a vector structure
 *  @return       pointer on the first item
 */
  void *
  vector_front( vector *self );

/**
 *  Returns a pointer to the last item
 *
 *  @param  self  a vector structure
 *  @return pointer on the last item
 */
  void *
  vector_back( vector *self );

/**
 *  Check if an item is contained within the vector.
 *
 *  @param  self  a vector structure
 *  @param  item  item to be searched in the vector
 *  @param  cmp   a pointer a comparison function
 *  @return       1 if item is contained within the vector, 0 otherwise
 */
  int
  vector_contains( vector *self,
                   void *item,
                   int (*cmp)(void *, void *) );

/**
 *  Checks whether the vector is empty.
 *
 *  @param  self  a vector structure
 *  @return       1 if the vector is empty, 0 otherwise
 */
  int
  vector_empty( vector *self );

/**
 *  Returns the number of items
 *
 *  @param  self  a vector structure
 *  @return       number of items
 */
  size_t
  vector_size( vector *self );

/**
 *  Reserve storage such that it can hold at last size items.
 *
 *  @param  self  a vector structure
 *  @param  size  the new storage capacity
 */
  void
  vector_reserve( vector *self,
                  size_t size );

/**
 *  Returns current storage capacity
 *
 *  @param  self  a vector structure
 *  @return       storage capacity
 */
  size_t
  vector_capacity( vector *self );

/**
 *  Decrease capacity to fit actual size.
 *
 *  @param  self  a vector structure
 */
  void
  vector_shrink( vector *self );

/**
 *  Removes all items.
 *
 *  @param  self  a vector structure
 */
  void
  vector_clear( vector *self );

/**
 *  Replace an item.
 *
 *  @param  self  a vector structure
 *  @param  index the index of the item to be replaced
 *  @param  item  the new item
 */
  void
  vector_set( vector *self,
              size_t index,
              void *item );

/**
 *  Erase an item.
 *
 *  @param  self  a vector structure
 *  @param  index the index of the item to be erased
 */
  void
  vector_erase( vector *self,
                size_t index );

/**
 *  Erase a range of items.
 *
 *  @param  self  a vector structure
 *  @param  first the index of the first item to be erased
 *  @param  last  the index of the last item to be erased
 */
  void
  vector_erase_range( vector *self,
                      size_t first,
                      size_t last );

/**
 *  Appends given item to the end of the vector.
 *
 *  @param  self a vector structure
 *  @param  item the item to be inserted
 */
  void
  vector_push_back( vector *self,
                    void *item );

/**
 *  Removes the last item of the vector.
 *
 *  @param  self a vector structure
 */
  void
  vector_pop_back( vector *self );

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
  vector_resize( vector *self,
                 size_t size );

/**
 *  Insert a single item at specified index.
 *
 *  @param  self  a vector structure
 *  @param  index location before which to insert item
 *  @param  item  the item to be inserted
 */
  void
  vector_insert( vector *self,
                 size_t index,
                 void *item );

/**
 *  Insert raw data at specified index.
 *
 *  @param  self  a vector structure
 *  @param  index location before which to insert item
 *  @param  data  a pointer to the items to be inserted
 *  @param  count the number of items to be inserted
 */
  void
  vector_insert_data( vector *self,
                      size_t index,
                      void * data,
                      size_t count );

/**
 *  Append raw data to the end of the vector.
 *
 *  @param  self  a vector structure
 *  @param  data  a pointer to the items to be inserted
 *  @param  count the number of items to be inserted
 */
  void
  vector_push_back_data( vector *self,
                         void * data, 
                         size_t count );

/**
 *  Sort vector items according to cmp.
 *
 *  @param  self  a vector structure
 *  @param  cmp   a pointer a comparison function
 */
  void
  vector_sort( vector *self,
               int (*cmp)(void *, void *) );

/** @} */

#endif /* __VECTOR_H__ */
