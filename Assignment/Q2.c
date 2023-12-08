//Including Header files
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define MAX 50

//DFS
int depth_search(int begin, int n, int mat_graph[MAX][MAX], int k)
{
    if (k == 0)
        return 1;

    int cnt_tot = 0, i = 1;

    for (; i <= n; ++i)
    {
        if (mat_graph[begin][i] == 1)
            cnt_tot += depth_search(i, n, mat_graph, k - 1);
    }

    return cnt_tot;
}
//Main function
int main(int argc, char **argv)
{

    int proc_num, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
    MPI_Status status;

    int flag = 5, flag1 = 10;
    int walk_tot = 0;
    int mat_graph[MAX][MAX] = {0};
    // Master Node
    if (rank == 0)
    {
        int n, e, k;
        scanf("%d %d %d", &n, &e, &k);

        // Randomly generating graph
        int s, d;
        for (int i = 0; i < e; i++)
        {
            s=rand()%n;
            d=rand()%n;
            mat_graph[s][d] = 1;
        }
        // Allocating nodes for processes
        int num_node_on_proc = n / (proc_num - 1);
        int num_node_remain = n % (proc_num - 1);

        // setting bounds
        int boundl = 1, boundu = num_node_on_proc;

        if (num_node_remain != 0)
        {
            boundu = num_node_remain;

            // Master calculates offsets
            for (int node = boundl; node <= boundu; ++node)
                walk_tot += depth_search(node, n, mat_graph, k);

            boundl = boundu + 1;
            boundu = boundl + num_node_on_proc - 1;
        }

        // Communicating: Sending data to workers
        for (int dest = 1; dest < proc_num; dest++)
        {
            MPI_Send(&boundl, 1, MPI_INT, dest, flag, MPI_COMM_WORLD);
            MPI_Send(&boundu, 1, MPI_INT, dest, flag + 1, MPI_COMM_WORLD);
            MPI_Send(&k, 1, MPI_INT, dest, flag + 2, MPI_COMM_WORLD);
            MPI_Send(&n, 1, MPI_INT, dest, flag + 3, MPI_COMM_WORLD);
            boundl = boundu + 1;
            boundu += num_node_on_proc;
        }
    }

    clock_t start = clock();
    // Sending(Broadcast) graph to all nodes
    MPI_Bcast(&mat_graph, MAX * MAX, MPI_INT, 0, MPI_COMM_WORLD);

    // Worker process
    if (rank > 0)
    {

        // Recieving Data from Master Node
        int boundl, boundu, k, n;
        MPI_Recv(&boundl, 1, MPI_INT, 0, flag, MPI_COMM_WORLD, &status);
        MPI_Recv(&boundu, 1, MPI_INT, 0, flag + 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&k, 1, MPI_INT, 0, flag + 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&n, 1, MPI_INT, 0, flag + 3, MPI_COMM_WORLD, &status);
        int walk_curr = 0;

        // Calculating walks 
        for (int node = boundl; node <= boundu; node++)
        {
            int x = depth_search(node, n, mat_graph, k);
            walk_curr += x;
        }

        // Sending result data to master
        MPI_Send(&walk_curr, 1, MPI_INT, 0, flag1, MPI_COMM_WORLD);
    }
    // Master
    if (rank == 0)
    {

        // Recieve walks from respective worker nodes
        int x;
        for (int src = 1; src < proc_num; ++src)
        {
            MPI_Recv(&x, 1, MPI_INT, src, flag1, MPI_COMM_WORLD, &status);
            walk_tot += x;
        }
        printf("%d\n", walk_tot);
        clock_t end = clock();
        printf("Time taken in seconds is: %lf\n", ((double)(end - start) /CLOCKS_PER_SEC));

    }
    MPI_Finalize();
    return 0;
}