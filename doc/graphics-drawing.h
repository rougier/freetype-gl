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
 * @page graphics-drawing Drawing primitives
 *
 * The graphics module draws the OpenGL primitive objects by a mode denoted by
 * the constants
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
 * Use <tt>\ref vertex_buffer_draw</tt> to draw a primitive. The following
 * example draws two points at coordinates (10, 15) and (30, 35):
 *
 * \code
 *  int v[] =  { 10,15, 30,35 };
 *  vertex_buffer_draw ("v2i", GL_POINTS, v, 2 ); 
 * \endcode
 */
