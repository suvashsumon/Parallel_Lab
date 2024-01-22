#include<bits/stdc++.h>
using namespace std;

__global__ void matrix_multiplication(int* A, int* B, int* C, int* DP)
{
    int N = DP[0];
    int M = DP[1];
    int P = DP[2];
    int number_of_matrix = DP[3];
    int number_of_core = DP[4];

    int i = threadIdx.x;
    int start_matrix_index = (number_of_matrix/number_of_core)*i;
    int end_matrix_index = (number_of_matrix/number_of_core)*(i+1);
    if(i==number_of_core-1) end_matrix_index = number_of_matrix;

    for(int x = start_matrix_index; x<end_matrix_index; x++)
    {
        int addA = x*N*M;
        int addB = x*M*P;
        int addC = x*N*P;

        for(int i=0; i<N; i++)
        {
            for(int j=0; j<P; j++)
            {
                int sum = 0;
                for(int k=0; k<M; k++)
                {
                    sum += A[addA + i*M + k] * B[addB + k*P + j];
                }
                C[addC + i*P+j] = sum;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    cout << "Enter the dimension of the matrix N, M, P : " << endl;
    int N, M, P;
    cin >> N >> M >> P;
    cout << "Enter the number of matrix : " << endl;
    int number_of_matrix;
    cin >> number_of_matrix;
    
    int *A, *B, *C;
    int a_element = number_of_matrix*N*M;
    int b_element = number_of_matrix*M*P;
    int c_element = number_of_matrix*N*P;

    A = new int[a_element];
    B = new int[b_element];
    C = new int[c_element];

    for(int i=0; i<a_element; i++)
    {
        A[i] = 1;
    }
    for(int i=0; i<b_element; i++)
    {
        B[i] = 1;
    }

    int *DA, *DB, *DC;
    cudaMalloc(&DA, a_element*sizeof(int));
    cudaMalloc(&DB, b_element*sizeof(int));
    cudaMalloc(&DC, c_element*sizeof(int));

    cudaMemcpy(DA, A, a_element*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(DB, B, b_element*sizeof(int), cudaMemcpyHostToDevice);


    cout << "Enter the number of core you want to use : " << endl;
    int number_of_core;
    cin >> number_of_core;

    cudaEvent_t start, end;
    cudaEventCreate(&start);
    cudaEventCreate(&end);
    cudaEventRecord(start);

    int parameter[] = {N, M, P, number_of_matrix, number_of_core};
    int *DP;
    cudaMalloc(&DP, 5*sizeof(int));
    cudaMemcpy(DP, parameter, 5*sizeof(int), cudaMemcpyHostToDevice);

    matrix_multiplication <<< 1, number_of_core >>> (DA, DB, DC, DP);

    cudaDeviceSynchronize();

    cudaEventRecord(end);
    cudaEventSynchronize(end);

    float milisecond = 0;
    cudaEventElapsedTime(&milisecond, start, end);
    cout << "Time taken with " << number_of_core << " is : " << milisecond << " ms" << endl;

    cudaMemcpy(C, DC, c_element*sizeof(int), cudaMemcpyDeviceToHost);

    for(int i=0; i<N; i++)
    {
        for(int j=0; j<P; j++)
        {
            cout << C[i*P+j] << " ";
        }
        cout << endl;
    }

    return 0;
}