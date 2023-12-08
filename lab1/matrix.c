#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define R1 5		 // number of rows in Matrix-1
#define C1 5  	 // number of columns in Matrix-1
#define R2 5		 // number of rows in Matrix-2
#define C2 5		 // number of columns in Matrix-2

void mulMat(int mat1[][C1], int mat2[][C2],int N) {
    int rslt[R1][C2];

 
    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < C2; j++) {
            rslt[i][j] = 0;
 
            for (int k = 0; k < R2; k++) {
                rslt[i][j] += mat1[i][k] * mat2[k][j];
            }
 
            
        }
 
        
    }
  
  FILE *input4 = fopen("multiply.txt", "w");
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      int val = rslt[i][j];
      fprintf(input4, "%d ", val);
    }

    fprintf(input4, "\n");
  }
  fclose(input4);
  
}

void main(int argc, char** argv)
{
  int N = atoi(argv[1]);
  srand(time(0));
  int mat1[N][N];
  int mat2[N][N];
  int res[N][N]; 
  FILE *input1 = fopen("input_matrix1.txt", "w");
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      int val = rand() % 501;
      mat1[i][j]=val;
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
      mat2[i][j]=val;
      fprintf(input2, "%d ", val);
    }
    fprintf(input2, "\n");
  }

  fclose(input2);
  
  FILE *input3 = fopen("sum.txt", "w");
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      int val = mat1[i][j]+mat2[i][j];
      fprintf(input3, "%d ", val);
    }

    fprintf(input3, "\n");
  }
  fclose(input3);
  
  mulMat(mat1,mat2,N);
 
}
