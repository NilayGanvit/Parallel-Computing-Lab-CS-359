#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define N 4

int x_coords[N] = {0, 0, 1, 1}, y_coords[N] = {0, 1, 1, 0};

void *compute_area(void *);

int sample_points_per_thread = 100000;

int main()
{

    pthread_t p_threads[N];

    int hits[N];
    float a = 0;

    for (int i = 0; i < N; i++)
    {
        hits[i] = i;
        pthread_create(&p_threads[i], NULL, compute_area, (void *)&hits[i]);
    }
    for (int i = 0; i < N; i++)
    {
        pthread_join(p_threads[i], NULL);
        a += hits[i];
    }

    a = abs(a / 2);

    printf("Area: %f\n", a);
}

void *compute_area(void *s)
{
    int index, *p;
    p = (int *)s;
    index = *p;

    if (index < N - 1)
        *p = x_coords[index] * y_coords[index + 1] - x_coords[index + 1] * y_coords[index];
    else
        *p = x_coords[index] * y_coords[0] - x_coords[0] * y_coords[index];

    pthread_exit((void *)p);
}