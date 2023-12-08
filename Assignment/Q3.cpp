//Importing headers
#include <bits/stdc++.h>
#include <time.h>
#include <pthread.h>
using namespace std;

//Size of matrix
#define max 512
//mutex lock
pthread_mutex_t mutex_g;
//global variables
int ans_final = 0, n, thread_num, n_on_thread;
//array
int array_g[10000][2] = {0};
//calculating function
void *thread_func(void *s)
{

    int *index = (int *)s;
    int index_usr = *(index);
    int ans_curr = 0;
    //calculating and adding all to get final answer
    for (int i = 0; i < n_on_thread; ++i)
    {
        if (index_usr + i < n)
        {

            pthread_mutex_lock(&mutex_g); //lock

            ans_final += ((array_g[index_usr + i][0] * array_g[index_usr + i + 1][1]) - (array_g[index_usr + i][1] * array_g[index_usr + i + 1][0]));
            pthread_mutex_unlock(&mutex_g);
        }
    }

    pthread_exit(0);
}
//main function
int main(int argc, char **argv)
{

    pthread_t p_threads[max];
    pthread_attr_t attr;
    clock_t begin, end;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_mutex_init(&mutex_g, NULL);
    //size
    n = 10000;
    //initialing random
    for (int i = 0; i < n; ++i)
    {

        array_g[i][0] = rand() % 42;
        array_g[i][1] = rand() % 42;
    }
    array_g[n][0] = array_g[0][0];
    array_g[n][1] = array_g[0][1];
    //Start recording time
    begin = clock();
    double area_sequential = 0;
    //Calculating area
    for (int i = 0; i < n; ++i)
    {
        area_sequential += (array_g[i][0] * array_g[i + 1][1] - array_g[i][1] * array_g[i + 1][0]);
    }
    end = clock();
    double time_sequential = (double(end - begin)) / CLOCKS_PER_SEC;
    cout << "Sequential Area = " << area_sequential / 2 << endl;
    cout << "Time taken for Serial = " << time_sequential << endl;

    cout << "Threads = ";
    //user input
    cin >> thread_num;
    n_on_thread = n / thread_num;

    int *index_usr = new int(0);
    int num_index[thread_num] = {0};
    //Start recording time
    begin = clock();
    //Allocation of nodes
    for (int i = 0; i < thread_num; ++i)
    {
        num_index[i] = *(index_usr);
        *(index_usr) += n_on_thread;
    }
    //parallel execution
    for (int i = 0; i < thread_num; ++i)
    {

        pthread_create(&p_threads[i], &attr, thread_func, (void *)(&num_index[i]));
    }
    for (int i = 0; i < thread_num; ++i)
    {
        pthread_join(p_threads[i], NULL);
    }
    end = clock();
    double parallel_time = (double(end - begin)) / CLOCKS_PER_SEC;
    cout << "Area Parallel = " << double(ans_final / 2) << endl;
    cout << "Time taken by parallel = " << parallel_time << endl;
    cout << "Speedup = " << double(time_sequential / parallel_time) << endl;
}