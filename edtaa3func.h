/*
 * Copyright 2009 Stefan Gustavson (stefan.gustavson@gmail.com)
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY STEFAN GUSTAVSON ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL STEFAN GUSTAVSON OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Stefan Gustavson.
 *
 *
 * edtaa3()
 *
 * Sweep-and-update Euclidean distance transform of an
 * image. Positive pixels are treated as object pixels,
 * zero or negative pixels are treated as background.
 * An attempt is made to treat antialiased edges correctly.
 * The input image must have pixels in the range [0,1],
 * and the antialiased image should be a box-filter
 * sampling of the ideal, crisp edge.
 * If the antialias region is more than 1 pixel wide,
 * the result from this transform will be inaccurate.
 *
 * By Stefan Gustavson (stefan.gustavson@gmail.com).
 *
 * Originally written in 1994, based on a verbal
 * description of the SSED8 algorithm published in the
 * PhD dissertation of Ingemar Ragnemalm. This is his
 * algorithm, I only implemented it in C.
 *
 * Updated in 2004 to treat border pixels correctly,
 * and cleaned up the code to improve readability.
 *
 * Updated in 2009 to handle anti-aliased edges.
 *
 * Updated in 2011 to avoid a corner case infinite loop.
 *
 */
#ifndef __EDTAA3FUNC_H__
#define __EDTAA3FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
namespace ftgl {
#endif

void edtaa3(const double *img, double *gx, double *gy, int w, int h, short *distx, short *disty, double *dist);

#ifdef __cplusplus
}
}
#endif

#endif // __EDTAA3FUNC_H__
