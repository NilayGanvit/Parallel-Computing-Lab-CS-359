#include<stdio.h>
#include<stdlib.h>
#include<time.h>

//Mention the order of the square matrices as the first command line argument.

void main(int argc, char** argv)
{
  int N = atoi(argv[1]);
  srand(time(0));
  FILE *input1 = fopen("input_matrix1.txt", "w");
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      int val = rand() % 501;
      fprintf(input1, "%d ", val);
    }

    fprintf(input1, "\n");
  }

  fclose(input1);
  FILE *input2 = fopen("input_matrix2.txt", "w");
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      int val = rand() % 501;
      fprintf(input2, "%d ", val);
    }
    fprintf(input2, "\n");
  }

  fclose(input2);
}