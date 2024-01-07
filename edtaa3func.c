/*
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
 * Updated 2012 to change license from LGPL to MIT.
 *
 * Updated 2014 to fix a bug with the 'gy' gradient computation.
 *
 */

/*
 * Copyright (C) 2009-2012 Stefan Gustavson (stefan.gustavson@gmail.com)
 * The code in this file is distributed under the MIT license:
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <math.h>
#include <float.h>

#include "edtaa3func.h"

#define DISTMAX DBL_MAX
#define EPSILON DBL_EPSILON

#ifndef M_SQRT2
    #define SQRT2 1.41421356237309504880
#else
    #define SQRT2 M_SQRT2
#endif

/*
 * A somewhat tricky function to approximate the distance to an edge in a
 * certain pixel, with consideration to either the local gradient (gx,gy)
 * or the direction to the pixel (dx,dy) and the pixel greyscale value a.
 * The latter alternative, using (dx,dy), is the metric used by edtaa2().
 * Using a local estimate of the edge gradient (gx,gy) yields much better
 * accuracy at and near edges, and reduces the error even at distant pixels
 * provided that the gradient direction is accurately estimated.
 */
static double edgedf (double dx, double dy, double a)
{
    double df, gx, gy, a1;

    const double scale = 1.0 / sqrt (dx * dx + dy * dy);

    /* Everything is symmetric wrt sign and transposition,
     * so move to first octant (gx>=0, gy>=0, gx>=gy) to
     * avoid handling all possible edge directions.
     */
    dx = fabs (dx);
    dy = fabs (dy);

    if (dx < dy) {
        gx = dy * scale;
        gy = dx * scale;
    } else {
        gx = dx * scale;
        gy = dy * scale;
    }

    a1 = 0.5 * gy;

    if ((a * gx) < a1) { // 0 <= a < a1
        df = 0.5 * (gx + gy) - sqrt (2.0 * gx * gy * a);
    } else if ((a * gx) < (gx - a1)) { // a1 <= a <= 1-a1
        df = (0.5 - a) * gx;
    } else { // 1-a1 < a <= 1
        df = -0.5 * (gx + gy) + sqrt (2.0 * gx * gy * (1.0 - a));
    }

    return df;
}

void edtaa3 (const double* img, double* gx, double* gy, int w, int h, short* distx, short* disty, double* dist)
{
    int x, y, i, k, c;
    double d, olddist;

    /* Initialize index offsets for the current image width */
    const int offset_u = -w;
    const int offset_ul = -w - 1;
    const int offset_ur = -w + 1;
    const int offset_l = -1;
    const int offset_r = 1;
    const int offset_d = w;
    const int offset_dl = w - 1;
    const int offset_dr = w + 1;

    /* Initialize image gradients and distances */
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            i = y * w + x;

            distx[i] = 0;
            disty[i] = 0;
            gx[i] = 0.0;
            gy[i] = 0.0;

            if (img[i] == 0.0) {
                dist[i] = DISTMAX;
            } else if (img[i] < 1.0) {
                if (y == 0 || y == h - 1 || x == 0 || x == w - 1) {
                    dist[i] = 0.5 - img[i];
                } else {
                    gx[i] = -img[i-w-1] -SQRT2*img[i-1] -img[i+w-1] +img[i-w+1] +SQRT2*img[i+1] +img[i+w+1];
                    gy[i] = -img[i-w-1] -SQRT2*img[i-w] -img[i-w+1] +img[i+w-1] +SQRT2*img[i+w] +img[i+w+1];
                    dist[i] = edgedf (gx[i], gy[i], img[i]);
                }
            }
        }
    }

#define COMPARE(offset, dx, dy) \
    k = i + offset; \
    c = k - distx[k] - disty[k] * w; \
    if (img[c] > 0.0) { \
        const int x = distx[k] + dx; \
        const int y = disty[k] + dy; \
        if (x == 0 && y == 0) { \
            d = edgedf (gx[c], gy[c], img[c]); \
        } else if (x == 0 || y == 0) { \
            d = sqrt (x*x + y*y) + 0.5f - img[c]; \
        } else { \
            d = sqrt (x*x + y*y) + edgedf (x, y, img[c]); \
        } \
        if (d < olddist - EPSILON) { \
            distx[i] = x; \
            disty[i] = y; \
            dist[i] = d; \
            olddist = d; \
    }}

    /* Perform the transformation */

    /* Scan rows, except first row */
    for (int y = 1; y < h; y++) {

        /* Move index to leftmost pixel of current row */
        int i = y * w;

        /* Scan right, propagate distances from above & left */

        /* Leftmost pixel is special, has no left neighbors */
        olddist = dist[i];

        if (olddist > 0.0) { // If non-zero distance or not set yet
            COMPARE (offset_u,   0,  1)
            COMPARE (offset_ur, -1,  1)
        }

        i++;

        /* Middle pixels have all neighbors */
        for (int x = 1; x < w - 1; x++, i++) {
            olddist = dist[i];

            if (olddist > 0.0) {
                COMPARE (offset_l,   1,  0)
                COMPARE (offset_ul,  1,  1)
                COMPARE (offset_u,   0,  1)
                COMPARE (offset_ur, -1,  1)
            }
        }

        /* Rightmost pixel of row is special, has no right neighbors */
        olddist = dist[i];

        if (olddist > 0.0) { // If not already zero distance
            COMPARE (offset_l,   1,  0)
            COMPARE (offset_ul,  1,  1)
            COMPARE (offset_u,   0,  1)
        }

        /* Move index to second rightmost pixel of current row. */
        /* Rightmost pixel is skipped, it has no right neighbor. */
        i = y * w + w - 2;

        /* Scan left, propagate distance from right */
        for (int x = w - 2; x >= 0; x--, i--) {
            olddist = dist[i];

            if (olddist > 0.0) {
                COMPARE (offset_r,  -1,  0)
            }
        }
    }

    /* Scan rows in reverse order, except last row */
    for (int y = h - 2; y >= 0; y--) {

        /* Move index to rightmost pixel of current row */
        int i = y * w + w - 1;

        /* Scan left, propagate distances from below & right */

        /* Rightmost pixel is special, has no right neighbors */
        olddist = dist[i];

        if (olddist > 0.0) { // If not already zero distance
            COMPARE (offset_d,   0, -1)
            COMPARE (offset_dl,  1, -1)
        }

        i--;

        /* Middle pixels have all neighbors */
        for (int x = w - 2; x > 0; x--, i--) {
            olddist = dist[i];

            if (olddist > 0.0) {
                COMPARE (offset_r,  -1,  0)
                COMPARE (offset_dr, -1, -1)
                COMPARE (offset_d,   0, -1)
                COMPARE (offset_dl,  1, -1)
            }
        }

        /* Leftmost pixel is special, has no left neighbors */
        olddist = dist[i];

        if (olddist > 0.0) { // If not already zero distance
            COMPARE (offset_r,  -1,  0)
            COMPARE (offset_dr, -1, -1)
            COMPARE (offset_d,   0, -1)
        }

        /* Move index to second leftmost pixel of current row. */
        /* Leftmost pixel is skipped, it has no left neighbor. */
        i = y * w + 1;

        /* Scan right, propagate distance from left */
        for (int x = 1; x < w; x++, i++) {
            olddist = dist[i];

            if (olddist > 0.0) {
                COMPARE (offset_l,   1,  0)
            }
        }
    }

    /* The transformation is completed. */
}
