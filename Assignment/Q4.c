//Header files needed for the program
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// Defining number of nodes
#define Num 500

// Define minimum function that will be used later on to calcualte minimum values between two numbers
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

// Define matrix of size Num * Num to store distances between nodes
// Initialize all distances to zero
int mat_init[Num][Num] = {0};

int main(int argc, char *argv[])
{
    int thread_num;
    int iter, dist, pivot;

    // Initialize the graph with random distances
    for (iter = 0; iter < Num; iter++)
    {
        for (dist = 0; dist < Num; dist++)
        {
            // Distance from node to same node is 0. So, skipping these elements
            if (iter != dist)
            {
                // Distances are generated to be between 0 and 19
                mat_init[iter][dist] = rand() % 20;
            }
        }
    }

    // Define time variable to record start time for execution of program
    double start_time = omp_get_wtime();

    for (pivot = 0; pivot < Num; pivot++)
    {
        int *mat_ptr = mat_init[pivot];
        for (iter = 0; iter < Num; iter++)
        {
            int *dp = mat_init[iter];
            for (dist = 0; dist < Num; dist++)
            {
                dp[dist] = min(dp[dist], dp[pivot] + mat_ptr[dist]);
            }
        }
    }

    double time = omp_get_wtime() - start_time;
    printf("Sequential time = %.2f sec\n", time);
    double seqt=time;

    for (thread_num = 2; thread_num <= 16; thread_num=thread_num*2)
    {
        // Define different number of threads
        omp_set_num_threads(thread_num);

        // Define iterator to iterate over distance matrix
        // Define time variable to record start time for execution of program
        double start_time = omp_get_wtime();

/* Taking a node as mediator
check if indirect distance between source and distance via mediator
is less than direct distance between them */
#pragma omp parallel shared(mat_init)
        for (pivot = 0; pivot < Num; pivot++)
        {
            int *mat_ptr = mat_init[pivot];
#pragma omp parallel for private(iter, dist) schedule(dynamic)
            for (iter = 0; iter < Num; iter++)
            {
                int *dp = mat_init[iter];
                for (dist = 0; dist < Num; dist++)
                {
                    dp[dist] = min(dp[dist], dp[pivot] + mat_ptr[dist]);
                }
            }
        }

        double time = omp_get_wtime() - start_time;
        printf("parallel time for  %d threads = %.2f sec and speedup = %2f\n", thread_num, time,seqt/time);
    }
    return 0;
}