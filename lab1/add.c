#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char **argv)
{
  int np, pid, numworkers, source, dest, rows, offset, i, j, k, N;
  N = atoi(argv[1]);
  double a[N][N], b[N][N], c[N][N];
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  numworkers = np - 1;

  /*---------------------------- master ----------------------------*/
  if (pid == 0)
  {

    printf("Taking input of matrices of size %dX%d from input files\n", N, N);
    FILE *input1 = fopen("input_matrix1.txt", "r");
    FILE *input2 = fopen("input_matrix2.txt", "r");

    printf("Reading first matrix\n");
    for (i = 0; i < N; i++)
    {
      for (j = 0; j < N; j++)
      {
        fscanf(input1, "%lf", &a[i][j]);
      }
    }

    printf("Reading second matrix\n");
    for (i = 0; i < N; i++)
    {
      for (j = 0; j < N; j++)
      {
        fscanf(input2, "%lf", &b[i][j]);
      }
    }

    printf("Adding matrices...\n");
    clock_t start = clock();

    /* send matrix data to the worker tasks */
    rows = N / numworkers;
    offset = 0;

    for (dest = 1; dest <= numworkers; dest++)
    {
      MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
      MPI_Send(&a[offset][0], rows * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
      MPI_Send(&b[offset][0], rows * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
      offset = offset + rows;
    }

    /* wait for results from all worker tasks */
    for (i = 1; i <= numworkers; i++)
    {
      source = i;
      MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
      MPI_Recv(&c[offset][0], rows * N, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &status);
    }

    clock_t end = clock();

    FILE *output = fopen("resultantMatrix_sum.txt", "w");

    printf("Writing the resultant matrix to the output file\n");
    for (i = 0; i < N; i++)
    {
      for (j = 0; j < N; j++)
        fprintf(output, "%.2f   ", c[i][j]);
      fprintf(output, "\n");
    }

    printf("Done\n");
    printf("Time taken in seconds is: %lf\n", ((double)(end - start) / CLOCKS_PER_SEC));
  }

  /*---------------------------- worker----------------------------*/
  if (pid > 0)
  {
    source = 0;
    MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&a, rows * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&b, rows * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);

    /* Matrix addition */
    for (k = 0; k < N; k++)
      for (i = 0; i < rows; i++)
      {
        c[i][k] = 0.0;
        c[i][k] = a[i][k] + b[i][k];
      }

    MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(&c, rows * N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
  }

  MPI_Finalize();
}
