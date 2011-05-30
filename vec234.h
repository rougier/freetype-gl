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
#ifndef __VEC234_H__
#define __VEC234_H__

typedef struct { union { float data[2];
                         struct { float x, y;                 };
                         struct { float width, height;        }; }; } vec2;

typedef struct { union { float data[3];
                         struct { vec2 xy; float z;           };
                         struct { float x, y, z;              };
                         struct { float w, h, d;              };
                         struct { float width, height, depth; };
                         struct { float r, g, b;              }; }; } vec3;

typedef struct { union { float data[4];
                         struct { vec3 xyz; float w;          };
                         struct { vec3 rgb; float a;          };
                         struct { vec2 xy; float z; float w;  };
                         struct { float x, y, z, w;           };
                         struct { float u0, v0, u1, v1;       };
                         struct { float s, t, p, q;           };
                         struct { float x, y, width, height;  };
                         struct { float r, g, b, a;           }; }; } vec4;

typedef struct { union { int data[2]; 
                         struct { int x, y;                  };
                         struct { int width, height;         }; }; } ivec2;

typedef struct { union { int data[3];
                         struct { ivec2 xy; int z;           };
                         struct { int x, y, z;               };
                         struct { int w, h, d;               };
                         struct { int width, height, depth;  };
                         struct { int r, g, b;               }; }; } ivec3;

typedef struct { union { int data[4];
                         struct { ivec3 xyz; int w;          };
                         struct { ivec3 rgb; int a;          };
                         struct { ivec2 xy; int z; int w;    };
                         struct { float u0, v0, u1, v1;      };
                         struct { float s, t, p, q;          };
                         struct { int x, y, z, w;            };
                         struct { int x, y, width, height;   };
                         struct { int r, g, b, a;            }; }; } ivec4;

#endif /* __VEC234_H__ */
