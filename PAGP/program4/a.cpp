#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
#include <error.h>   // if this gives error on your system, see note below
#include <omp.h>

int main(int argc, char *argv[])
{
    int nt = 4;                       // number of threads
    int tid, tmp, red, green, blue;
    int color, x, y, w, h;
    tmp = red = green = blue = color = x = y = w = h = 0;

    char *iname = NULL;
    char *oname = NULL;
    gdImagePtr img;
    FILE *fp = NULL;

    if (argc != 3)
        error(1, 0, "format : %s input.png output.png", argv[0]);
    else {
        iname = argv[1];
        oname = argv[2];
    }

    if ((fp = fopen(iname, "rb")) == NULL)
        error(1, 0, "error : fopen : %s", iname);
    else {
        img = gdImageCreateFromPng(fp);
        fclose(fp);

        w = gdImageSX(img);
        h = gdImageSY(img);

        double t = omp_get_wtime();

        omp_set_num_threads(nt);

        /* try different schedules:
           static, dynamic, guided, different chunk sizes
        */
        #pragma omp parallel for private(y, color, red, green, blue, tmp, tid) \
                                 schedule(static,10)
        for (x = 0; x < w; x++) {
            for (y = 0; y < h; y++) {

                tid = omp_get_thread_num();

                color = gdImageGetPixel(img, x, y);
                red   = gdImageRed  (img, color);
                green = gdImageGreen(img, color);
                blue  = gdImageBlue (img, color);

                tmp = (red + green + blue) / 3;
                red = green = blue = tmp;   // gray value

                if (tid == 0) {
                    color = gdImageColorAllocate(img, red, 0, 0);      // red-ish
                    gdImageSetPixel(img, x, y, color);
                }
                if (tid == 1) {
                    color = gdImageColorAllocate(img, 0, green, 0);    // green-ish
                    gdImageSetPixel(img, x, y, color);
                }
                if (tid == 2) {
                    color = gdImageColorAllocate(img, 0, 0, blue);     // blue-ish
                    gdImageSetPixel(img, x, y, color);
                }
                if (tid == 3) {
                    color = gdImageColorAllocate(img, 0, 0, 0);        // black
                    gdImageSetPixel(img, x, y, color);
                }
            }
        }

        t = omp_get_wtime() - t;
        printf("\nTime taken : %lf  threads : %d\n", t, nt);

        if ((fp = fopen(oname, "wb")) == NULL)
            error(1, 0, "error : fopen : %s", oname);
        else {
            gdImagePng(img, fp);
            fclose(fp);
        }

        gdImageDestroy(img);
    }

    return 0;
}
