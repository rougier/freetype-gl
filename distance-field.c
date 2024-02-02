/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#include <math.h>
#include <float.h>
#include <stdlib.h>

#include "distance-field.h"
#include "edtaa3func.h"

double *
make_distance_mapd (double *data, unsigned int width, unsigned int height)
{
    const unsigned int size = width * height;
    short * xdist = (short *) malloc (size * sizeof (short));
    short * ydist = (short *) malloc (size * sizeof (short));
    double * gx      = (double *) malloc (size * sizeof (double));
    double * gy      = (double *) malloc (size * sizeof (double));
    double * outside = (double *) calloc (size, sizeof (double));
    double * inside  = (double *) calloc (size, sizeof (double));
    double vmin = DBL_MAX;
    unsigned int i;

    // Compute outside = edtaa3 (bitmap); % Transform background (0's)
    edtaa3 (data, gx, gy, width, height, xdist, ydist, outside);

    // Compute inside = edtaa3 (1-bitmap); % Transform foreground (1's)
    for (i=0; i<size; ++i)
        data[i] = 1.0 - data[i];

    edtaa3 (data, gx, gy, width, height, xdist, ydist, inside);

    // distmap = outside - inside; % Bipolar distance field
    for (i=0; i<size; ++i)
    {
        if (inside[i] < 0.0) inside[i] = 0.0;
        if (outside[i] < 0.0) outside[i] = 0.0;

        outside[i] -= inside[i];

        if (outside[i] < vmin) vmin = outside[i];
    }

    vmin = fabs (vmin);

    for (i=0; i<size; ++i)
    {
        if (outside[i] < -vmin)
            data[i] = 0.0;
        else if (outside[i] > +vmin)
            data[i] = 1.0;
        else
            data[i] = (outside[i] + vmin) / (2.0 * vmin);
    }

    free (xdist);
    free (ydist);
    free (gx);
    free (gy);
    free (outside);
    free (inside);

    return data;
}

unsigned char *
make_distance_mapb (const unsigned char *img,
                    unsigned int width, unsigned int height)
{
    const unsigned int size = width * height;
    double * data = (double *) malloc (size * sizeof (double));
    unsigned char *out = (unsigned char *) malloc (size * sizeof (unsigned char));
    unsigned int i;

    // Map values from 0 - 255 to 0.0 - 1.0
    for (i=0; i<size; ++i)
    {
        if (img[i] == 0)
            data[i] = 0.0;
        else if (img[i] == 255)
            data[i] = 1.0;
        else
            data[i] = (double) img[i] * (1.0 / 255.0);
    }

    data = make_distance_mapd (data, width, height);

    // map values from 0.0 - 1.0 to 0 - 255
    for (i=0; i<size; ++i)
        out[i] = (unsigned char) (255.0 * (1.0 - data[i]));

    free (data);

    return out;
}
