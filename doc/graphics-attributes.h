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
 * @page graphics-attributes Vertex attributes
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
 * The following attributes are normalised to the range [0, 1]. The value is
 * used as-is if the data type is floating-point. If the data type is byte,
 * short or int, the value is divided by the maximum value representable by
 * that type. For example, unsigned bytes are divided by 255 to get the
 * normalised value.
 *
 *  - %Color
 *  - Secondary color
 *  - Generic attributes with the "n" format given.
 *
 * Up to 8 generic attributes can be specified per vertex, and can be used by
 * shader programs for any purpose (they are ignored in the fixed-function
 * pipeline). For the other attributes, consult the OpenGL programming guide
 * for details on their effects.
 *
 * When using the draw and related functions, attribute data is specified
 * alongside the vertex position data. The following example reproduces the two
 * points from the previous page, except that the first point is blue and the
 * second green:
 *
 * \code
 * struct vertex { float x,y;
 *                 char  r,g,b;} v[] =  { {10,15, 0,0,255},
 *                                        {30,35, 0,255,0} };
 * vertex_buffer_draw ("v2f:c3b", GL_POINTS, v, 2 ); 
 * \endcode
 *
 * It is an error to provide more than one set of data for any attribute, or to
 * mismatch the size of the initial data with the number of vertices specified
 * in the first argument.
 *
 */
