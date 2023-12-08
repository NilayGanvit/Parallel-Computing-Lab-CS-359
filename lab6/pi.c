#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#define NUM_THREADS 32
#define usr_num 32

void *compute_pi(void *s)
{
    int seed, i, *hit_pointer;
    double x_coord, y_coord;
    int local_hits;
    hit_pointer = (int *)s;
    seed = *hit_pointer;
    local_hits = 0;
    int sample_points_per_thread = usr_num;
    for (i = 0; i < sample_points_per_thread; i++)
    {
        x_coord = (double)(rand_r(&seed)) / (RAND_MAX)-0.5;
        y_coord = (double)(rand_r(&seed)) / (RAND_MAX)-0.5;
        if ((x_coord * x_coord + y_coord * y_coord) < 0.25)
            local_hits++;
    }
    *hit_pointer = local_hits;
    pthread_exit(0);
}

int main()
{

    pthread_t p_threads[NUM_THREADS];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int hits[NUM_THREADS];
    int total_hits = 0;
    for (int i = 0; i < NUM_THREADS; i++)
    {
        hits[i] = i;
        pthread_create(&p_threads[i], &attr, compute_pi,
                       (void *)&hits[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(p_threads[i], NULL);
        total_hits += hits[i];
    }
    int sample_points_per_thread = usr_num;
    int total_points = sample_points_per_thread * NUM_THREADS;
    double pi = (double)4 * (double)total_hits / (double)total_points;
    printf("estimated value of pi using %d sample points per %d threads is %.4f\n", sample_points_per_thread,NUM_THREADS, pi);
}