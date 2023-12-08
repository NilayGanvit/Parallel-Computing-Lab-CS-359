#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char **argv){

	int rank_usr, proc_num;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank_usr);
	MPI_Request req[proc_num + 1];
	MPI_Status status;
	int num;
	int Rec_Dat[3];
	int Send_Dat[proc_num + 1][3];
	int filled[1000];
	int weights[1000];
	int mat_graph[1000][1000];
	int num_max_colour = 0;
	int vertices, edges;

	int l = 0;
	for (l = 0; l < 1000; l++)
		filled[l] = -1;

	if (rank_usr == 0)
	{
		scanf("%d", &vertices);
		scanf("%d", &edges);
		int i, j;

		for (i = 0; i < vertices; i++)
		{
			weights[i] = rand() % 42 + rand() % 42 + rand() % 42 + rand() % 42;
			filled[i] = -1;
		}

		int start, end;
		for (i = 0; i < edges; i++)
		{
			scanf("%d %d", &start, &end);
			mat_graph[start][end] = 1;
			mat_graph[end][start] = 1;
		}
		clock_t begin = clock();
		int max = -1;

		for (i = 0; i < vertices; i++)
		{
			max = 0;
			for (j = 0; j < vertices; j++)
			{
				if (mat_graph[i][j] != 0)
					max++;
			}
			if (max > num_max_colour)
				num_max_colour = max;
		}

		num_max_colour += 1;

		int num_vetices_per_proc = vertices / (proc_num - 1);
		int num_vetices_remaining = vertices / (proc_num - 1) + (vertices % (proc_num - 1));
		int startVertex = 0, endVertex;

		for (i = 1; i < proc_num - 1; i++)
		{
			MPI_Send(&vertices, 1, MPI_INT, i, i, MPI_COMM_WORLD);
			int k = 0;
			for (k = 0; k < vertices; k++)
			{
				MPI_Send(&mat_graph[k], vertices, MPI_INT, i, i, MPI_COMM_WORLD);
			}

			endVertex = startVertex + num_vetices_per_proc - 1;
			Send_Dat[i][0] = startVertex;
			Send_Dat[i][1] = endVertex;
			Send_Dat[i][2] = num_max_colour;
			MPI_Send(Send_Dat[i], 3, MPI_INT, i, i, MPI_COMM_WORLD);
			MPI_Send(filled, vertices, MPI_INT, i, i, MPI_COMM_WORLD);
			MPI_Send(weights, vertices, MPI_INT, i, i, MPI_COMM_WORLD);
			startVertex = endVertex + 1;
		}
		MPI_Send(&vertices, 1, MPI_INT, i, i, MPI_COMM_WORLD);

		int k = 0;
		for (k = 0; k < vertices; k++)
			MPI_Send(&mat_graph[k], vertices, MPI_INT, i, i, MPI_COMM_WORLD);

		endVertex = startVertex + num_vetices_remaining - 1;
		Send_Dat[i][0] = startVertex;
		Send_Dat[i][1] = endVertex;
		Send_Dat[i][2] = num_max_colour;
		MPI_Send(Send_Dat[i], 3, MPI_INT, i, i, MPI_COMM_WORLD);
		MPI_Send(filled, vertices, MPI_INT, i, i, MPI_COMM_WORLD);
		MPI_Send(weights, vertices, MPI_INT, i, i, MPI_COMM_WORLD);

		for (i = 1; i <= proc_num - 1; i++)
			MPI_Recv(filled, vertices, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		max = -1;
		for (i = 0; i < vertices; i++)
		{
			if (filled[i] > max)
				max = filled[i];
		}
		printf("%d\n", max + 1);
		for (i = 0; i < vertices; i++)
			printf("%d\n", filled[i]);
		
		clock_t final = clock();
        printf("Time taken in seconds is: %lf\n", ((double)(final/8 - begin/8) /CLOCKS_PER_SEC));
	}
	else
	{
		int vertices;
		MPI_Recv(&vertices, 3, MPI_INT, 0, rank_usr, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int mat_graph[vertices][vertices];

		int i, j;
		for (i = 0; i < vertices; i++)
			MPI_Recv(&mat_graph[i], vertices, MPI_INT, 0, rank_usr, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		MPI_Request receiver_req;
		MPI_Status receiver_status;

		MPI_Recv(Rec_Dat, 3, MPI_INT, 0, rank_usr, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int filled[vertices], weights[vertices];
		MPI_Recv(filled, vertices, MPI_INT, 0, rank_usr, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(weights, vertices, MPI_INT, 0, rank_usr, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int vertex_curr;
		int round = 0;
		while (round < vertices)
		{
			int all_filled = 1;
			for (vertex_curr = Rec_Dat[0]; vertex_curr <= Rec_Dat[1]; vertex_curr++)
			{
				if (filled[vertex_curr] == -1)
				{
					all_filled = 0;

					int weight_usr = weights[vertex_curr];

					int max_wt_usr = 1;

					int adjacent;

					for (adjacent = 0; adjacent < vertices; adjacent++)
					{
						if (adjacent != vertex_curr)
						{
							if (mat_graph[vertex_curr][adjacent] == 1 && filled[adjacent] == -1 && weights[adjacent] > weights[vertex_curr])
							{
								max_wt_usr = 0;
								break;
							}
						}
					}
					if (max_wt_usr)
					{
						int colour_filled = -1;

						for (colour_filled = 0; colour_filled < Rec_Dat[2]; colour_filled++)
						{
							int avail_col = 1;
							for (adjacent = 0; adjacent < vertices; adjacent++)
							{
								if (mat_graph[vertex_curr][adjacent] == 1 && filled[adjacent] != -1 && colour_filled == filled[adjacent])
								{
									avail_col = 0;
									break;
								}
							}
							if (avail_col == 1)
							{
								filled[vertex_curr] = colour_filled;
								break;
							}
						}
					}
				}
			}
			int m;
			for (m = 1; m <= proc_num - 1; m++)
			{
				if (m != rank_usr)
					MPI_Send(filled, vertices, MPI_INT, m, m, MPI_COMM_WORLD);
			}

			for (m = 1; m <= proc_num - 1; m++)
			{
				int col_usr[vertices], weight_curr_usr[vertices];
				if (m != rank_usr)
					MPI_Recv(col_usr, vertices, MPI_INT, m, rank_usr, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				int count = 0;
				for (count = 0; count < vertices; count++)
				{
					if (filled[count] == -1 && col_usr[count] != -1 && col_usr[count] < Rec_Dat[2] && col_usr[count] >= 0)
						filled[count] = col_usr[count];
				}
			}
			int count;
			round++;
			sleep(3);
		}
		MPI_Send(filled, vertices, MPI_INT, 0, rank_usr, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}