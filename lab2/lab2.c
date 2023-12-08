#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

int Default = 9999999;
int cnt_proccess;
int rank_user;

int range_rtn(int argc, char **argv){
   if(argc<2||!argv[1]){
      return Default;
   }
   
   int range = atoi(argv[1]);
   
   if(range<2*cnt_proccess){
      return Default;
   }
   
   return range;

}

int prime_check(int n){
   int high,d;
   high = (int)(sqrt((double)n)+0.0001);
   for(d=2;d<=high;d++)
      if(n%d==0){
         return 0;
   }
   return 1;
}

void worker(int argc,char**argv){

   int range=range_rtn(argc,argv);  
   double chng=(double)range/cnt_proccess;
   int lower; 
   int upper; 
   
   lower=(rank_user==0)?2:(int)(rank_user*chng)+1;
   upper=(rank_user== cnt_proccess-1)?range:(int)((rank_user+1)*chng);

   int count=0;    
   int i;

   if(rank_user!=0){
      for(i=lower;i<= upper;i++)
         if (prime_check(i)){
            count++;
            }
      int data_to_send[3];
      data_to_send[0]=count;
      data_to_send[1]=lower;
      data_to_send[2]=upper;
      MPI_Send(data_to_send,3,MPI_INT,0,0,MPI_COMM_WORLD);
   }else{
      double start=MPI_Wtime();
      for(i=lower;i<=upper;i++)
         if(prime_check(i)){
            count++;
            }
      printf("\nThere are %d primes between 0 and %d counted by process 18.\n", count, upper);
      int ct;
      for (ct = 1; ct < cnt_proccess; ct++) {
         int data_received[3];
         MPI_Status status;
         MPI_Recv( data_received, 3, MPI_INT, MPI_ANY_SOURCE,
                        MPI_ANY_TAG, MPI_COMM_WORLD, &status );
         count += data_received[0];
         printf("\nThere are %d primes between %d and %d counted by process %d.\n", data_received[0], data_received[1], data_received[2],status.MPI_SOURCE);
      }
      printf("\nThe workers counted %d primes till %d.\n", count,range);
      double elapsed = MPI_Wtime() - start;
      printf("\nTime taken:  %1.3f seconds.\n", elapsed);
   }
}


int main(int argc, char **argv) {

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank_user);
   MPI_Comm_size(MPI_COMM_WORLD, &cnt_proccess);
   
   worker(argc, argv);
   
   MPI_Finalize();

}
