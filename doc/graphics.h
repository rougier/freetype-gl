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
 * @page graphics Graphics
 *
 * At the lowest level, cyglet uses OpenGL to draw in windows. The OpenGL
 * interface is exposed via the graphics module (see The OpenGL interface).
 *
 * However, using the OpenGL interface directly for drawing graphics is
 * difficult and inefficient. The graphics module provides a simpler means for
 * drawing graphics that uses vertex arrays and vertex buffer objects
 * internally to deliver better performance.
 *
 *
 * - \subpage graphics-drawing
 * - \subpage graphics-attributes
 * - \subpage graphics-buffer
 *    - \ref graphics-buffer-1
 *    - \ref graphics-buffer-2
 *    - \ref graphics-buffer-3
 * - \subpage graphics-batch
 *    - \ref graphics-batch-1
 *    - \ref graphics-batch-2
 *    - \ref graphics-batch-3
 */
