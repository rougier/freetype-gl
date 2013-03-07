/* ============================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * ----------------------------------------------------------------------------
 * Copyright 2011,2012 Nicolas P. Rougier. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Nicolas P. Rougier.
 * ============================================================================
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mat4.h"

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

mat4 *
mat4_new( void )
{
    mat4 *self = (mat4 *) malloc( sizeof(mat4) );
    return self;

}

void
mat4_set_zero( mat4 *self )
{
    assert( self );

    memset( self, 0, sizeof( mat4 ));
}

void
mat4_set_identity( mat4 *self )
{
    assert( self );

    memset( self, 0, sizeof( mat4 ));
    self->m00 = 1.0;
    self->m11 = 1.0;
    self->m22 = 1.0;
    self->m33 = 1.0;
}

void
mat4_multiply( mat4 *self, mat4 *other )
{
    mat4 m;
    size_t i;

    assert( self );
    assert( other );

    for( i=0; i<4; ++i )
    {
        m.data[i*4+0] =
            (self->data[i*4+0] * other->data[0*4+0]) +
            (self->data[i*4+1] * other->data[1*4+0]) +
            (self->data[i*4+2] * other->data[2*4+0]) +
            (self->data[i*4+3] * other->data[3*4+0]) ;

        m.data[i*4+1] =
            (self->data[i*4+0] * other->data[0*4+1]) +
            (self->data[i*4+1] * other->data[1*4+1]) +
            (self->data[i*4+2] * other->data[2*4+1]) +
            (self->data[i*4+3] * other->data[3*4+1]) ;
        
        m.data[i*4+2] =
            (self->data[i*4+0] * other->data[0*4+2]) +
            (self->data[i*4+1] * other->data[1*4+2]) +
            (self->data[i*4+2] * other->data[2*4+2]) +
            (self->data[i*4+3] * other->data[3*4+2]) ;
        
        m.data[i*4+3] =
            (self->data[i*4+0] * other->data[0*4+3]) +
            (self->data[i*4+1] * other->data[1*4+3]) +
            (self->data[i*4+2] * other->data[2*4+3]) +
            (self->data[i*4+3] * other->data[3*4+3]) ;
    }
    memcpy( self, &m, sizeof( mat4 ) );

}


void
mat4_set_orthographic( mat4 *self,
                       float left,   float right,
                       float bottom, float top,
                       float znear,  float zfar )
{
    assert( self );
    assert( right  != left );
    assert( bottom != top  );
    assert( znear  != zfar );

    mat4_set_zero( self );

    self->m00 = +2.0/(right-left);
    self->m30 = -(right+left)/(right-left);
    self->m11 = +2.0/(top-bottom);
    self->m31 = -(top+bottom)/(top-bottom);
    self->m22 = -2.0/(zfar-znear);
    self->m32 = -(zfar+znear)/(zfar-znear);
    self->m33 = 1.0;
}

void
mat4_set_perspective( mat4 *self,
                      float fovy,  float aspect,
                      float znear, float zfar)
{
    float h, w;
   
    assert( self );
    assert( znear != zfar );

    h = tan(fovy / 360.0 * M_PI) * znear;
    w = h * aspect;
 
    mat4_set_frustum( self, -w, w, -h, h, znear, zfar );
}

void
mat4_set_frustum( mat4 *self,
                  float left,   float right,
                  float bottom, float top,
                  float znear,  float zfar )
{
    
    assert( self );
    assert( right  != left );
    assert( bottom != top  );
    assert( znear  != zfar );

    mat4_set_zero( self );

    self->m00 = +2.0*znear/(right-left);
    self->m20 = (right+left)/(right-left);

    self->m11 = +2.0*znear/(top-bottom);
    self->m31 = (top+bottom)/(top-bottom);

    self->m22 = -(zfar+znear)/(zfar-znear);
    self->m32 = -2.0*znear/(zfar-znear);

    self->m23 = -1.0;
}

void
mat4_set_rotation( mat4 *self,
                   float angle,
                   float x, float y, float z)
{
    float c, s, norm;
  
    assert( self );

    c = cos( M_PI*angle/180.0 );
    s = sin( M_PI*angle/180.0 );
    norm = sqrt(x*x+y*y+z*z);

    x /= norm; y /= norm; z /= norm;

    mat4_set_identity( self );

    self->m00 = x*x*(1-c)+c;
    self->m10 = y*x*(1-c)-z*s;
    self->m20 = z*x*(1-c)+y*s;

    self->m01 =  x*y*(1-c)+z*s;
    self->m11 =  y*y*(1-c)+c;
    self->m21 =  z*y*(1-c)-x*s;

    self->m02 = x*z*(1-c)-y*s;
    self->m12 = y*z*(1-c)+x*s;
    self->m22 = z*z*(1-c)+c;
}

void
mat4_set_translation( mat4 *self,
                      float x, float y, float z)
{
    assert( self );

    mat4_set_identity( self );
    self-> m30 = x;
    self-> m31 = y;
    self-> m32 = z;
}

void
mat4_set_scaling( mat4 *self,
                  float x, float y, float z)
{
    assert( self );

    mat4_set_identity( self );
    self-> m00 = x;
    self-> m11 = y;
    self-> m22 = z;
}

void
mat4_rotate( mat4 *self,
             float angle,
             float x, float y, float z)
{
    mat4 m;
    
    assert( self );

    mat4_set_rotation( &m, angle, x, y, z);
    mat4_multiply( self, &m );
}

void
mat4_translate( mat4 *self,
                float x, float y, float z)
{
    mat4 m;
    assert( self );

    mat4_set_translation( &m, x, y, z);
    mat4_multiply( self, &m );
}

void
mat4_scale( mat4 *self,
            float x, float y, float z)
{
    mat4 m;
    assert( self );

    mat4_set_scaling( &m, x, y, z);
    mat4_multiply( self, &m );
}
