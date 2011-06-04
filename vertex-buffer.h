// ============================================================================
// Freetype GL - An ansi C OpenGL Freetype engine
// Platform:    Any
// API version: 1.0
// WWW:         http://www.loria.fr/~rougier/freetype-gl
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program. If not, see <http://www.gnu.org/licenses/>.
// ============================================================================
#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__

#if defined(__APPLE__)
    #include <Glut/glut.h>
#else
    #include <GL/glut.h>
#endif
#include "vector.h"

#define MAX_VERTEX_ATTRIBUTE 8


/**
 * @defgroup Graphics Drawing primitives
 *
 * The graphics module draws the OpenGL primitive objects by a mode
 * denoted by the constants
 * 
 * - GL_POINTS
 * - GL_LINES
 * - GL_LINE_LOOP
 * - GL_LINE_STRIP
 * - GL_TRIANGLES
 * - GL_TRIANGLE_STRIP
 * - GL_TRIANGLE_FAN
 * - GL_QUADS
 * - GL_QUAD_STRIP
 * - GL_POLYGON
 *
 * See the <a href="http://opengl.org/documentation/red_book/">OpenGL
 * Programming Guide</a> for a description of each of mode.
 *
 * Each primitive is made up of one or more vertices. Each vertex is specified
 * with either 2, 3 or 4 components (for 2D, 3D, or non-homogeneous
 * coordinates). The data type of each component can be either int or float.
 *
 * Use vertex_buffer_draw to draw a primitive. The following example draws two
 * points at coordinates (10, 15) and (30, 35):
 *
 * \code
 *  float v[] =  { 10,15, 30,35 };
 *  vertex_buffer_draw ("v2i", GL_POINTS, v, 2, 0, 0 ); 
 * \endcode
 *
 * A vertex buffer is a list of vertices with attributes, stored in an
 * efficient manner that's suitable for direct upload to the video card.
 *
 * On newer video cards (supporting OpenGL 1.5 or later) the data is actually
 * stored in video memory. Creating a VertexList for a set of attributes is
 * straightforward:
 *
 * \code
 *   typedef struct { float x,y,z; } vec3;
 *   typedef struct { vec3 position, normal, color;} vertex;
 *   vec3 v[] = { { 1, 1, 1},  {-1, 1, 1},  {-1,-1, 1}, { 1,-1, 1},
 *                { 1,-1,-1},  { 1, 1,-1},  {-1, 1,-1}, {-1,-1,-1} };
 *   vec3 n[] = { { 0, 0, 1},  { 1, 0, 0},  { 0, 1, 0} ,
 *                {-1, 0, 1},  { 0,-1, 0},  { 0, 0,-1} };
 *   vec3 c[] = { {1, 1, 1},  {1, 1, 0},  {1, 0, 1},  {0, 1, 1},
 *                {1, 0, 0},  {0, 0, 1},  {0, 1, 0},  {0, 0, 0} };
 *   vertex vertices[24] =  {
 *     {v[0],n[0],c[0]}, {v[1],n[0],c[1]}, {v[2],n[0],c[2]}, {v[3],n[0],c[3]},
 *     {v[0],n[1],c[0]}, {v[3],n[1],c[3]}, {v[4],n[1],c[4]}, {v[5],n[1],c[5]},
 *     {v[0],n[2],c[0]}, {v[5],n[2],c[5]}, {v[6],n[2],c[6]}, {v[1],n[2],c[1]},
 *     {v[1],n[3],c[1]}, {v[6],n[3],c[6]}, {v[7],n[3],c[7]}, {v[2],n[3],c[2]},
 *     {v[7],n[4],c[7]}, {v[4],n[4],c[4]}, {v[3],n[4],c[3]}, {v[2],n[4],c[2]},
 *     {v[4],n[5],c[4]}, {v[7],n[5],c[7]}, {v[6],n[5],c[6]}, {v[5],n[5],c[5]} };
 *   GLuint indices[24] = { 0, 1, 2, 3,    4, 5, 6, 7,   8, 9,10,11,
 *                          12,13,14,15,  16,17,18,19,  20,21,22,23 };
 *   cube = vertex_buffer_new_from_data("v3f:n3f:c3f",
 *                                      24, vertices, 24, indices );
 *   ...
 *
 *   vertex_buffer_render( cube, GL_QUADS, "vnc" );
 *
 *   ...
 *
 * \endcode
 *
 * Any OpenGL drawing primitive is available:
 *
 *  - GL_POINTS
 *  - GL_LINES
 *  - GL_LINE_LOOP
 *  - GL_LINE_STRIP
 *  - GL_TRIANGLES
 *  - GL_TRIANGLE_STRIP
 *  - GL_TRIANGLE_FAN
 *  - GL_QUADS
 *  - GL_QUAD_STRIP
 *  - GL_POLYGON
 *
 * See the <a href="http://opengl.org/documentation/red_book/">OpenGL
 * Programming Guide</a> for a description of each of mode.
 *
 * Each primitive is made up of one or more vertices. Each vertex is specified
 * with either 2, 3 or 4 components (for 2D, 3D, or non-homogeneous
 * coordinates). The data type of each component can be either int or float.
 *
 * Because vertex data can be supplied in several forms, a "format string" is
 * required. In the above case, the format string is "v3f:c3i", meaning the
 * vertex position data has three components with float type while the color
 * data has three components with int type.
 *
 * Besides the required vertex position, vertices can have several other
 * numeric attributes. Each is specified in the format string with a letter,
 * the number of components and the data type.
 *
 * Each of the attributes is described in the table below with the set of valid
 * format strings written as a regular expression (for example, "v[234][if]"
 * means "v2f", "v3i", "v4f", etc. are all valid formats).
 *
 * Some attributes have a "recommended" format string, which is the most
 * efficient form for the video driver as it requires less conversion.
 *
 * <table>
 *   <tr>
 *     <th>%Attribute </th>
 *     <th>Formats</th>
 *     <th>Recommended</th>
 *   </tr>
 *   <tr>
 *     <td>Vertex position</td>
 *     <td>"v[234][sifd]"</td>
 *     <td>"v[234]f"</td>
 *   </tr>
 *   <tr>
 *     <td> %Color             </td>
 *     <td> "c[34][bBsSiIfd]" </td>
 *     <td> "c[34]B"          </td>
 *   </tr>
 *   <tr>
 *     <td> Edge flag </td>
 *     <td> "e1[bB]"  </td>
 *     <td>           </td>
 *   </tr>
 *   <tr>
 *    <td> Fog coordinate     </td>
 *    <td> "f[1234][bBsSiIfd]"</td>
 *    <td>                    </td>
 *   </tr>
 *   <tr>
 *     <td> Normal      </td>
 *     <td> "n3[bsifd]" </td>
 *     <td> "n3f"       </td>
 *   </tr>
 *   <tr>
 *     <td> Secondary color   </td>
 *     <td> "s[34][bBsSiIfd]" </td>
 *     <td> "s[34]B"          </td>
 *   </tr>
 *   <tr>
 *     <td> Texture coordinate </td>
 *     <td> "t[234][sifd]"     </td>
 *     <td> "t[234]f"          </td>
 *   </tr>
 *   <tr>
 *     <td> Generic attribute             </td>
 *     <td> "[0-15]g(n)?[1234][bBsSiIfd]" </td> 
 *     <td>                             </td>
 *   </tr>
 * </table>
 *
 * The possible data types that can be specified in the format string are described below.
 *
 * <table>
 *   <tr>
 *     <th> Format   </th>
 *     <th> Type     </th> 
 *     <th> GL Type  </th>
 *   </tr>
 *   <tr>
 *     <td> "b"               </td>
 *     <td> Signed byte       </td> 
 *     <td> GL_BYTE           </td>
 *   </tr>
 *   <tr>
 *     <td> "B"               </td>
 *     <td> Unsigned byte     </td> 
 *     <td> GL_UNSIGNED_BYTE  </td>
 *   </tr>
 *   <tr>
 *     <td> "s"               </td>
 *     <td> Signed short      </td> 
 *     <td> GL_SHORT          </td>
 *   </tr>
 *   <tr>
 *     <td> "S"               </td>
 *     <td> Unsigned short    </td> 
 *     <td> GL_UNSIGNED_SHORT </td>
 *   </tr>
 *   <tr>
 *     <td> "i"               </td>
 *     <td> Signed int        </td> 
 *     <td> GL_INT            </td>
 *   </tr>
 *   <tr>
 *     <td> "I"               </td>
 *     <td> Unsigned int      </td> 
 *     <td> GL_UNSIGNED_INT   </td>
 *   </tr>
 *   <tr>
 *     <td> "f"               </td>
 *     <td> Float             </td> 
 *     <td> GL_FLOAT          </td>
 *   </tr>
 *   <tr>
 *     <td> "d"               </td>
 *     <td> Double            </td> 
 *     <td> GL_DOUBLE     T   </td>
 *   </tr>
 * </table>
 *
 *
 * Up to 8 generic attributes can be specified per vertex, and can be used by
 * shader programs for any purpose (they are ignored in the fixed-function
 * pipeline). For the other attributes, consult the OpenGL programming guide
 * for details on their effects.
 *
 * It is an error to provide more than one set of data for any attribute, or to
 * mismatch the size of the initial data with the number of vertices specified
 * in the first argument.
 *
 *  @{
 */

/**
 *  Generic vertex attribute.
 */
typedef struct 
{
    /** a <a
     * href="http://www.opengl.org/sdk/docs/man/xhtml/glEnableClientState.xml">
     * client-side</a> capability. */
    GLenum target;

    /** Number of component. */
    GLint size;

    /** Data type. */
    GLenum type;

    /** Byte offset between consecutive attributes. */
    GLsizei stride;

    /** Pointer to the first component of the first attribute element in the
     *  array. */
    GLvoid * pointer;

    /** Pointer to the function that enable this attribute. */
    void ( * enable )(void *);
} VertexAttribute;



/**
 * Generic vertex buffer.
 */
typedef struct
{
    /** Format of the vertex buffer. */
    char * format;

    /** Vector of vertices. */
    Vector * vertices;

    /** GL id of the vertices buffer. */
    GLuint   vertices_id;

    /** Vector of indices. */
    Vector * indices;

    /** GL id of the indices buffer. */
    GLuint   indices_id;

    /** Whether the vertex buffer needs to be uploaded to GPU memory. */
    char dirty;

    /** Array of attributes. */
    VertexAttribute *attributes[MAX_VERTEX_ATTRIBUTE];
} VertexBuffer;



/**
 * @name General
 *
 * @{ 
 */

/**
 * Creates an empty vertex buffer.
 *
 * @param  format a string describing vertex format.
 * @return        an empty vertex buffer.
 */
  VertexBuffer *
  vertex_buffer_new( char *format );


/**
 * Creates a vertex buffer from data.
 *
 * @param  format   a string describing vertex format.
 * @param  vcount   number of vertices
 * @param  vertices raw vertices data
 * @param  icount   number of vertices
 * @param  indices  raw indices data
 * @return          an empty vertex buffer.
 */

  VertexBuffer *
  vertex_buffer_new_from_data( char *format,
                               size_t vcount,
                               void * vertices,
                               size_t icount,
                               GLuint * indices );

/**
 * Deletes vertex buffer and releases GPU memory.
 *
 * @param  self  a vertex buffer
 */
  void
  vertex_buffer_delete( VertexBuffer *self );

/** @} **/



/**
 * @name Rendering
 *
 * @{ 
 */

/**
 * Render vertex buffer.
 *
 * @param  self  a vertex buffer
 * @param  mode  render mode
 * @param  what  attributes to be rendered
 */
  void
  vertex_buffer_render ( VertexBuffer *self,
                         GLenum mode,
                         char *what );

/**
 * Upload buffer to GPU memory.
 *
 * @param  self  a vertex buffer
 */
  void
  vertex_buffer_upload( VertexBuffer *self );

/** @} **/



/**
 * @name Modifiers
 *
 * @{ 
 */

/**
 * Appends a single index at the end of the buffer.
 *
 * @param  self  a vertex buffer
 * @param  index index to be appended
 */
  void
  vertex_buffer_push_back_index ( VertexBuffer *self,
                                  GLuint index );

/**
 * Appends a single vertex at the end of the buffer.
 *
 * @param  self   a vertex buffer
 * @param  vertex vertex to be appended
 */
  void
  vertex_buffer_push_back_vertex ( VertexBuffer *self,
                                   void *vertex );

/**
 * Appends indices at the end of the buffer.
 *
 * @param  self    a vertex buffer
 * @param  indices indices to be appended
 * @param  count   number of indices to be appended
 */
  void
  vertex_buffer_push_back_indices ( VertexBuffer *self,
                                    GLuint *indices,
                                    size_t count );

/**
 * Appends vertices at the end of the buffer.
 *
 * @param  self     a vertex buffer
 * @param  vertices vertices to be appended
 * @param  count    number of vertices to be appended
 */
  void
  vertex_buffer_push_back_vertices ( VertexBuffer *self,
                                     void *vertices,
                                     size_t count );

/**
 * Appends indices at the end of the buffer.
 *
 * @param  self    a vertex buffer
 * @param  index   location before which to insert indices
 * @param  indices indices to be appended
 * @param  count   number of indices to be appended
 */
  void
  vertex_buffer_insert_indices ( VertexBuffer *self,
                                 size_t index,
                                 GLuint *indices,
                                 size_t count );

/**
 * Appends indices at the end of the buffer.
 *
 * @param  self     a vertex buffer
 * @param  index    location before which to insert vertices
 * @param  vertices vertices to be appended
 * @param  count    number of vertices to be appended
 *
 * @note
 * Indices after index will be increased by count. 
 */
  void
  vertex_buffer_add_vertices ( VertexBuffer *self,
                               size_t index,
                               void *vertices,
                               size_t count );

/** @} **/



/**
 * @name Attributes
 *
 * @{
 */

/**
 * Create an attribute from the given parameters.
 *
 * @param target a <a
 *               href="http://www.opengl.org/sdk/docs/man/xhtml/glEnableClientState.xml">
 *               client-side</a> capability
 *
 * @param size    number of component
 * @param type    data type
 * @param stride  byte offset between consecutive attributes.
 * @param pointer pointer to the first component of the first attribute
 *                element in the array.
 * @return        a new initialized vertex attribute.
 */
VertexAttribute *
vertex_attribute_new( GLenum target,
                      GLint size,
                      GLenum type,
                      GLsizei stride,
                      GLvoid *pointer );

/**
 * Create an attribute from the given description.
 *
 * @param format Format string specifies the format of a vertex attribute.
 *
 * @note
 *
 * Format strings have the following (BNF) syntax:
 *
 * <tt>
 * attribute ::= ( name | index 'g' 'n'? ) count type
 * </tt>
 *
 * name describes the vertex attribute, and is one of the following constants
 * for the predefined attributes:
 *
 * - \c c Vertex color
 * - \c e Edge flag
 * - \c f Fog coordinate
 * - \c n Normal vector
 * - \c s Secondary color
 * - \c t Texture coordinate
 * - \c v Vertex coordinate
 *
 * You can alternatively create a generic indexed vertex attribute by
 * specifying its index in decimal followed by the constant \c g. For example,
 * \c 0g specifies the generic vertex attribute with index 0. If the optional
 * constant \c n is present after the \c g, the attribute is normalised to the
 * range [0, 1] or [-1, 1] within the range of the data type.
 *
 * \c count gives the number of data components in the attribute. For example, a
 * 3D vertex position has a count of 3. Some attributes constrain the possible
 * counts that can be used; for example, a normal vector must have a count of 3.
 *
 * type gives the data type of each component of the attribute. The following
 * types can be used:
 * - \c b GLbyte
 * - \c B GLubyte
 * - \c s GLshort
 * - \c S GLushort
 * - \c i GLint
 * - \c I GLuint
 * - \c f GLfloat
 * - \c d GLdouble
 *
 * Some attributes constrain the possible data types; for example, normal
 * vectors must use one of the signed data types. The use of some data types,
 * while not illegal, may have severe performance concerns. For example, the
 * use of GLdouble is discouraged, and colours should be specified with
 * GLubyte.
 *
 * Whitespace is prohibited within the format string.
 *
 * Some examples follow:
 *
 * - \c v3f 3-float vertex position
 * - \c c4b 4-byte colour
 * - \c 1eb Edge flag
 * - \c 0g3f 3-float generic vertex attribute 0
 * - \c 1gn1i Integer generic vertex attribute 1, normalized to [-1, 1]
 * - \c 2gn4B 4-byte generic vertex attribute 2, normalized to [0, 1] (because
 *    the type is unsigned)
 *
 */
  VertexAttribute *
  vertex_attribute_parse( char *format );

/**
 * Enable the position vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_position_enable( VertexAttribute *attr );

/**
 * Enable the normal vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_normal_enable( VertexAttribute *attr );

/**
 * Enable the color vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_color_enable( VertexAttribute *attr );

/**
 * Enable the texture vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_tex_coord_enable( VertexAttribute *attr );

/**
 * Enable the fog vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_fog_coord_enable( VertexAttribute *attr );

/**
 * Enable the edge flag vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_edge_flag_enable( VertexAttribute *attr );

/**
 * Enable the secondary color vertex attribute.
 *
 * @param attr  a vertex attribute
 */
  void
  vertex_attribute_secondary_color_enable( VertexAttribute *attr );

/** @} **/



/**
 * @name Helper functions
 *
 * @{
 */

/**
 * Returns the GL enum type correspond to given character.
 *
 * @param ctype  character type
 * @return       GL enum type
 */
  GLenum
  GL_TYPE( char ctype );

/**
 * Get the GL name of the given target.
 *
 * @param  ctarget  a char describing target ( one of v,c,e,f,n,s,t)
 * @return          the associated GL target
 */
  GLenum
  GL_VERTEX_ATTRIBUTE_TARGET( char ctarget );

/**
 * Returns the size of a given GL enum type.
 *
 * @param  gtype a GL enum type
 * @return       the size of the given type
 */
  GLuint
  GL_TYPE_SIZE( GLenum gtype );

/**
 * Returns the literal string of given GL enum type.
 *
 * @param  gtype a GL enum type
 * @return       the literal string describing the type
 */
  char *
  GL_TYPE_STRING( GLenum gtype );

/** @} **/

/** @} **/

#endif /* __VERTEX_BUFFER_H__ */
