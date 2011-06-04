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

/**
 * @page graphics-buffer Vertex buffers
 *
 * There is a significant overhead in using <tt>\ref vertex_buffer_draw</tt>
 * and <tt>\ref vertex_buffer_draw_indexed</tt> due to cyglet interpreting and
 * formatting the vertex data for the video device. Usually the data drawn in
 * each frame (of an animation) is identical or very similar to the previous
 * frame, so this overhead is unnecessarily repeated.
 *
 * A VertexBuffer is a list of vertices and their attributes, stored in an
 * efficient manner that's suitable for direct upload to the video card. On
 * newer video cards (supporting OpenGL 1.5 or later) the data is actually
 * stored in video memory.
 *
 * Create a VertexBuffer for a set of attributes and initial data with <tt>\ref
 * vertex_buffer_new_with_data</tt>. The following example creates a vertex
 * buffer with the two coloured points used in the previous page:
 *
 * \code
 * vertex_list = pyglet.graphics.vertex_list(2,
 * ('v2i', (10, 15, 30, 35)),
 *   ('c3B', (0, 0, 255, 0, 255, 0))
 * )
 * \endcode
 *
 * To draw the vertex list, call the <tt>\ref vertex_buffer_render</tt> method:
 *
 * \code
 * vertex_buffer_render( buffer, GL_POINTS, "vc" )
 * \endcode
 *
 * Note that the primitive mode is given to the draw method, not the vertex
 * list constructor. Otherwise the vertex_list method takes the same arguments
 * as pyglet.graphics.draw, including any number of vertex attributes.
 *
 *
 * @section graphics-buffer-1 Updating vertex data
 *
 * The data in a vertex buffer can be modified. Each vertex attribute (including
 * the vertex position) appears as an attribute on the VertexList object. The
 * attribute names are given in the following table.
 *
 * <table>
 *   <tr>
 *     <th>%VertexAttribute </th>
 *     <th>Associated get/set function</th>
 *   </tr>
 *   <tr>
 *   <td> Vertex position </td>
 *   <td>
 *   - verter_buffer_get_position
 *   - verter_buffer_set_position
 *   </td>
 *   </tr>
 *   <tr>
 *   <td> Vertex color </td>
 *   <td>
 *   - verter_buffer_get_color
 *   - verter_buffer_set_color
 *   </td>


 *   </tr>
 *   <tr>
 *   <td> Vertex color</td>
 *   <td> verter_buffer_[get/set]_color </td>
 *   </tr>
 * </table>
 *
 * @section graphics-buffer-2 Data usage
 *
 * @section graphics-buffer-3 Indexed vertex buffers
 */
