#include<bits/stdc++.h>
#include<mpi.h>
using namespace std;

int *take_input(int n)
{
    int *a = new int[n];
    for(int i=0; i<n; i++)
    {
        a[i] = rand()%10;
    }
    return a;
}

void send_a_number(int value, int to)
{
    MPI_Send(&value, 1, MPI_INT, to, 0, MPI_COMM_WORLD);
}

void send_a_array(int* arr, int sz, int to)
{
    send_a_number(sz, to);
    MPI_Send(arr, sz, MPI_INT, to, 0, MPI_COMM_WORLD);
}

int receive_a_number(int from)
{
    int value;
    MPI_Recv(&value, 1, MPI_INT, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return value;
}

vector<int>receive_a_array(int from)
{
    int length = receive_a_number(from);
    vector<int>ans(length);
    MPI_Recv(&ans[0], length, MPI_INT, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return ans;
}

vector<int>matrix_multiplication(int* A, int* B, int N, int M, int P, int number_of_matrix)
{
    vector<int>ans(N*P*number_of_matrix);
    for(int i=0; i<number_of_matrix; i++)
    {
        int addA = i * N * M;
        int addB = i * M * P;
        int addC = i * N * P;

        for(int j=0; j<N; j++)
        {
            for(int k=0; k<P; k++)
            {
                int sum = 0;
                for(int row=0; row<M; row++)
                {
                    sum+=A[addA+j*M+row] * B[addB+P*row+k];
                }
                ans[addC+j*P+k] = sum;
            }
        }
    }
    return ans;
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int number_of_thread;
    int thread_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &thread_id);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_thread);

    if(thread_id==0)
    {
        int N, M, P;
        cout << "Enter the dimension of the matrix (M N P) : " << endl;
        cin >> N >> M >> P;
        int number_of_matrix;
        cout << "Enter the number of matrix : " << endl;
        cin >> number_of_matrix;
        auto A = take_input(N * M * number_of_matrix);
        auto B = take_input(M * P * number_of_matrix);
        

        clock_t start, end;
        start = clock();

        for(int i=1; i<number_of_thread; i++)
        {
            int start_index = i * (number_of_matrix/number_of_thread);
            int end_index = (i+1) * (number_of_matrix/number_of_thread);
            if(i==number_of_thread-1) end_index = number_of_matrix;

            int number_of_matrix_for_process = end_index - start_index;
            send_a_number(number_of_matrix_for_process, i);

            send_a_number(N, i);
            send_a_number(M, i);
            send_a_number(P, i);

            int start_port = start_index * N * M;
            int end_port = end_index * N * M;
            send_a_array(A+start_port, end_port-start_port, i);

            start_port = start_index * M * P;
            end_port = end_index * M * P;
            send_a_array(B+start_port, end_port-start_port, i);
        }

        auto ans = matrix_multiplication(A, B, N, M, P, number_of_matrix/number_of_thread);
        end = clock();

        for(int i=1; i<number_of_thread; i++)
        {
            auto temp = receive_a_array(i);
            ans.insert(ans.end(), temp.begin(), temp.end());
        }

        for(int i=0; i<N; i++)
        {
            for(int j=0; j<P; j++)
            {
                cout << ans[i * P+j] << " ";
            }
            cout << endl;
        }

        double time_taken = double(end-start)/double(CLOCKS_PER_SEC);
        cout << "Time taken  : " << time_taken << "sec" << endl;
    }
    else
    {
        int number_of_matrix = receive_a_number(0);
        int N = receive_a_number(0);
        int M = receive_a_number(0);
        int P = receive_a_number(0);

        vector<int>A = receive_a_array(0);
        vector<int>B = receive_a_array(0);
        auto ans = matrix_multiplication(&A[0], &B[0], N, M, P, number_of_matrix);
        send_a_array(&ans[0], ans.size(), 0);
    }
    MPI_Finalize();
    return 0;
}