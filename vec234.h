/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * -------------------------------------------------------------------------
 * Copyright 2011, 2012 Nicolas P. Rougier. All rights reserved.
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
 * ========================================================================= */
#pragma once
#ifndef __VEC234_H__
#define __VEC234_H__

/**
 * Tuple of 4 floats
 */
typedef struct {
    union {
        /** */
        float data[4];
        /** */
        float rgba[4];
        /** */
        float xyz[3];
        /** */
        float rgb[3];
        struct {
            /** First component */
            union { float x;               float r; float red;   };
            /** Second component */
            union { float y;               float g; float green; };
            /** Third component */
            union { float z; float width;  float b; float blue;  };
            /** Fourth component */
            union { float w; float height; float a; float alpha; };
        };
    };
} vec4;

/**
 * Tuple of 3 floats
 */
typedef struct {
    union {
        /** */
        float data[3];
        /** */
        float rgb[3];
        /** */
        float xyz[3];
        struct {
            /** First component */
            union { float x; float width;  float r; float red;   };
            /** Second component */
            union { float y; float height; float g; float green; };
            /** Third component */
            union { float z; float depth;  float b; float blue;  };
        };
    };
} vec3;

/**
 * Tuple of 2 floats
 */
typedef struct {
    union {
        /** */
        float data[2];
        /** */
        float xy[2];
        struct {
            /** First component */
            union { float x; float width;  float l; float luminance; };
            /** Second component */
            union { float y; float height; float a; float alpha;     };
        };
    };
} vec2;

/**
 * Tuple of 4 ints
 */
typedef struct {
    union {
        /** */
        int data[4];
        /** */
        int rgba[4];
        /** */
        int xyz[3];
        /** */
        int rgb[3];
        struct {
            /** First component */
            union { int x;             int r; int red;   };
            /** Second component */
            union { int y;             int g; int green; };
            /** Third component */
            union { int z; int width;  int b; int blue;  };
            /** Fourth component */
            union { int w; int height; int a; int alpha; };
        };
    };
} ivec4;

/**
 * Tuple of 3 ints
 */
typedef struct {
    union {
        /** */
        int data[3];
        /** */
        int rgb[3];
        /** */
        int xyz[3];
        /** */
        int xy[2];
        struct {
            /** First component */
            union { int x; int width;  int r; int red;   };
            /** Second component */
            union { int y; int height; int g; int green; };
            /** Fourth component */
            union { int z; int depth;  int b; int blue;  };
        };
    };
} ivec3;

/**
 * Tuple of 2 ints
 */
typedef struct {
    union {
        /** */
        int data[2];
        /** */
        int xy[2];
        struct {
            /** First component */
            union { int x; int width;  int l; int luminance; };
            /** Second component */
            union { int y; int height; int a; int alpha;     };
        };
    };
} ivec2;



#endif /* __VEC234_H__ */
