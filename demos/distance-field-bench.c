/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
// query performance counter scale
double qpcscale;
#else
#include <time.h>
#endif

// Utility
static double* get_background (const unsigned char* image, int size);
static double* get_foreground (double* data, int size);
static unsigned char* get_distmap (double* outside, double* inside, int size);

static void dump (const char* s, const unsigned char* data, int w, int h);

static void get_process_time (double* t);
static double dumptime (const char* s, double begin, double end);
static double show_time (const char* s, double t1, double t2, int w, int h);

static void difference (
    int i, int verbose,
    const unsigned char* image, int width, int height,
    const char* name1, const unsigned char* img1,
    const char* name2, const unsigned char* img2);

// Original
void computegradient_orig (const double *img, int w, int h, double *gx, double *gy);
void edtaa3_orig (const double *img, double *gx, double *gy, int w, int h, short *distx, short *disty, double *dist);

#include "edtaa3orig.h"

unsigned char* benchoriginal (const unsigned char* image, int w, int h)
{
    const int size = w * h;

    short* distx = malloc (size * sizeof(short));
    short* disty = malloc (size * sizeof(short));
    double* gx = calloc (size, sizeof(double));
    double* gy = calloc (size, sizeof(double));
    double* outside = calloc (size, sizeof(double));
    double* inside = calloc (size, sizeof(double));

    double* data = get_background (image, size);

    computegradient_orig (data, w, h, gx, gy);
    edtaa3_orig (data, gx, gy, w, h, distx, disty, outside);
    
    data = get_foreground (data, size);

    memset (gx, 0, size * sizeof(double));
    memset (gy, 0, size * sizeof(double));

    computegradient_orig (data, w, h, gx, gy);
    edtaa3_orig (data, gx, gy, w, h, distx, disty, inside);

    free (data);
    free (gy);
    free (gx);
    free (disty);
    free (distx);

    unsigned char* map = get_distmap (outside, inside, size);

    free (inside);
    free (outside);

    return map;
}

// Optimized
#include "../edtaa3func.c"

unsigned char* benchoptimized (const unsigned char* image, int w, int h)
{
    const int size = w * h;

    short* distx = malloc (size * sizeof(short));
    short* disty = malloc (size * sizeof(short));
    double* gx = malloc (size * sizeof(double));
    double* gy = malloc (size * sizeof(double));
    double* outside = calloc (size, sizeof(double));
    double* inside = calloc (size, sizeof(double));

    double* data = get_background (image, size);

    edtaa3 (data, gx, gy, w, h, distx, disty, outside);
    
    data = get_foreground (data, size);

    edtaa3 (data, gx, gy, w, h, distx, disty, inside);

    free (data);
    free (gy);
    free (gx);
    free (disty);
    free (distx);

    unsigned char* map = get_distmap (outside, inside, size);

    free (inside);
    free (outside);

    return map;
}

typedef struct {
    const unsigned char* image;
    int width;
    int height;
} benchmark_args_t;

static double benchmark (
    const char* s, unsigned char** distmap,
    unsigned char* (*func)(const unsigned char*, int, int), benchmark_args_t* args)
{
    double begin, end;

    get_process_time (&begin);
    *distmap = func (args->image, args->width, args->height);
    get_process_time (&end);

    return dumptime (s, begin, end);
}

int main (int argc, char** argv)
{
    unsigned char *orig, *optim;
    double optimRatio = 0.0;

#if defined(_WIN32) || defined(_WIN64)
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency (&frequency);
    qpcscale = 1.0e+9 / frequency.QuadPart; 
#endif

// Glyph images
#include "image.h"

    for (int i = 0; i < imageDataSize; ++i)
    {
        printf ("\n# Image %d (Char '%c')\n", i, imageData[i].glyph);

        const unsigned char* image = imageData[i].data;
        const int width = imageData[i].width;
        const int height = imageData[i].height;

        benchmark_args_t args = { image, width, height };

        const double origTime = benchmark ("Original\t", &orig, benchoriginal, &args);
        const double optimTime = benchmark ("Optimized\t", &optim, benchoptimized, &args);

        optimRatio += show_time ("Optmized", origTime, optimTime, width, height);

        difference (i, 1, image, width, height, "Original", orig, "Optimized", optim);

        free (optim);
        free (orig);
    }
    printf ("\nOptimized average time ratio: %.2f\n", optimRatio / (double) imageDataSize);

    return 0;
}

static void difference (
    int i, int verbose,
    const unsigned char* image, int width, int height,
    const char* name1, const unsigned char* img1,
    const char* name2, const unsigned char* img2)
{
    if (memcmp (img1, img2, width*height) != 0)
    {
        printf ("Distance fields not equal for image %d\n", i);

        if (verbose) {
            dump ("Image", image, width, height);
            dump (name1, img1, width, height);
            dump (name2, img2, width, height);
        }

        int count = 0;
        int absmax = 0;

        printf ("Difference '%s' vs '%s'\n", name1, name2);

        for (int k = 0; k < width*height; ++k) {

            if (k > 0 && k % width == 0)
                printf ("\n");

            const int diff = img1[k] - img2[k];

            if (diff == 0) {
                printf ("%s", (img1[k] == 0 ? "  . " : "  * "));
            } else {
                printf ("  # ");
            }

            if (diff != 0) {
                count++;
                if (abs (diff) > absmax) absmax = abs (diff);
            }
        }
        printf ("\ncount = %d, max abs(diff) = %d\n", count, absmax);
    }
}

static void dump (const char* s, const unsigned char* data, int w, int h)
{
    if (s) printf ("%s\n", s);

    for (int i = 0; i < w*h; ++i) {
        if (i>0 && i%w==0) printf ("\n");
        printf ("%3d,", data[i]);
    }
    printf ("\n");
}

static void get_process_time (double* nanoseconds)
{
#if defined(_WIN32) || defined(_WIN64)
    LARGE_INTEGER t;
    QueryPerformanceCounter (&t);
    const double ns = t.QuadPart * qpcscale;
#else
    struct timespec t;
    clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &t);
    const double ns = t.tv_sec * 1.0e+9 + t.tv_nsec;
#endif
    *nanoseconds = ns;
}

static double dumptime (const char* s, double begin, double end)
{
    if (s) printf ("%s", s);

    const double ns = end - begin;
    printf ("Time measured: %.0f ns.\n", ns);

    return ns;
}

static double show_time (const char* s, double t1, double t2, int w, int h)
{
    const double timeRatio = t2 / t1;
    printf ("%12s: Time ratio: %.2f (%dx%d %d pixels)\n", s, timeRatio, w, h, w * h);

    return timeRatio;
}

static double* get_background (const unsigned char* image, int size)
{
    double* data = malloc (size * sizeof(double));

    for (int i = 0; i < size; ++i) {
        if (image[i] == 0)
            data[i] = 0.0;
        else if (image[i] == 255)
            data[i] = 1.0;
        else
            data[i] = (double) image[i] * (1.0 / 255.0);
    }

    return data;
}

static double* get_foreground (double* data, int size)
{
    for (int i = 0; i < size; ++i) {
        data[i] = 1.0 - data[i];
    }

    return data;
}

static unsigned char* get_distmap (double* outside, double* inside, int size)
{
    double vmin = DBL_MAX;

    for (int i = 0; i < size; ++i) {
        if (outside[i] < 0.0) outside[i] = 0.0;
        if (inside[i] < 0.0) inside[i] = 0.0;

        outside[i] -= inside[i];

        if (outside[i] < vmin) vmin = outside[i];
    }

    const double min = fabs (vmin);

    const double* distmap = outside;
    unsigned char* map = malloc (size * sizeof(unsigned char));

    for (int i = 0; i < size; ++i) {
        if (distmap[i] <= -min)
            map[i] = 255;
        else if (distmap[i] >= min)
            map[i] = 0;
        else {
            const double value = (distmap[i] + min) / (2.0 * min);
            map[i] = (unsigned char) (255.0 * (1.0 - value));
        }
    }

    return map;
}
