#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define SIZE 4000     // Matrix size

double a[SIZE][SIZE];
double b[SIZE][SIZE];
double c[SIZE][SIZE];
double d[SIZE][SIZE];

int main()
{
    int i, j, k;
    struct timeval tim;
    double t1, t2;
    double tmp;

    /* ----------------------------
       Initialize matrices
       ---------------------------- */
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
        {
            a[i][j] = (double)(i + j);
            b[i][j] = (double)(i - j);
            c[i][j] = 0.0;
            d[i][j] = 0.0;
        }

    /* ----------------------------
       Timestamp t1
       ---------------------------- */
    gettimeofday(&tim, NULL);
    t1 = tim.tv_sec + (tim.tv_usec / 1000000.0);

    /* ----------------------------
       OpenACC MATRIX MULTIPLICATION
       ---------------------------- */
    #pragma acc data copyin(a, b) copy(c)
    #pragma acc kernels
    #pragma acc loop tile(32, 32)
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
        {
            tmp = 0.0;

            #pragma acc loop reduction(+:tmp)
            for (k = 0; k < SIZE; k++)
                tmp += a[i][k] * b[k][j];

            c[i][j] = tmp;
        }

    /* ----------------------------
       Timestamp t2
       ---------------------------- */
    gettimeofday(&tim, NULL);
    t2 = tim.tv_sec + (tim.tv_usec / 1000000.0);

    printf("%.6lf seconds with OpenACC\n", (t2 - t1));

    /* ----------------------------
       Verify correctness using CPU
       ---------------------------- */
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
        {
            tmp = 0.0;
            for (k = 0; k < SIZE; k++)
                tmp += a[i][k] * b[k][j];

            d[i][j] = tmp;

            if (c[i][j] != d[i][j])
            {
                printf("Error at %d %d : %f %f\n", i, j, c[i][j], d[i][j]);
                exit(1);
            }
        }

    printf("OpenACC matrix multiplication test was successful!\n");

    return 0;
}
