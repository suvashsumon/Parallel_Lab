#include<bits/stdc++.h>
using namespace std;

__global__ void searchContact(char* contact_string, char* name, int* index, int contact_len, int name_len, int no_of_contact, int no_of_core)
{
    int idx = threadIdx.x;
    int start_index = idx*(no_of_contact/no_of_core);
    int end_index = (idx+1)*(no_of_contact/no_of_core);
    if(idx == no_of_core-1) end_index = contact_len;
    else end_index = index[end_index];

    int hash_count = start_index;
    int find = 0;
    for(int i=index[start_index]; i<end_index; i++)
    {
        if(contact_string[i]=='#') 
        {
            find = 0;
            hash_count++;
        }
        if(contact_string[i]==name[find]) find++;
        else find = 0;
        if(find == name_len) 
        {
            find = 0;
            index[hash_count] = -1;
        }
    }
}

__global__ void searchContact2(char *C, char *N, int *index, int contact_len, int name_len, int number_of_core, int number_of_contacts)
{
    int idx = threadIdx.x;
    int start_index = idx*(number_of_contacts/number_of_core);
    int end_index = (idx+1)*(number_of_contacts/number_of_core);
    if(idx==number_of_core-1) end_index = contact_len;
    else end_index = index[end_index];
    int find = 0;
    int hash_count = start_index;
    for(int i=index[start_index]; i<end_index; i++)
    {
        if((C[i]=='#'))
        {
            find = 0;
            hash_count++;
        }
        if(C[i]==N[find]) find++;
        else find = 0;
        if(find==name_len)
        {
            index[hash_count] = -1;
            find = 0;
        }
    }
}


int main(int argc, char* argv[])
{
    string name;
    cout << "Enter the name to search : ";
    cin >> name;
    for(auto &c : name) c = toupper(c);

    vector<string>contact_list, original_list;
    vector<string>file_names(argv+1, argv+argc);
    for(auto file_name : file_names)
    {
        ifstream file(file_name);
        string line;
        while(getline(file, line))
        {
            original_list.push_back(line);
            for(auto &c : line) c = toupper(c);
            contact_list.push_back(line);
        }
        file.close();
    }

    string contact_string = "";
    int number_of_contact = original_list.size();
    vector<int>contact_index(number_of_contact);
    for(int i=0; i<number_of_contact; i++)
    {
        contact_index[i] = contact_string.size();
        contact_string+=contact_list[i]+"#";
    }

    cout << "Enter the nubmer of core : ";
    int no_of_core;
    cin >> no_of_core;

    /// alocating memory
    char *DC, *DN;
    int *index;
    int contact_len = contact_string.size()+1;
    int name_len = name.size();
    cudaMalloc(&DC, contact_len);
    cudaMemcpy(DC, &contact_string[0], contact_len, cudaMemcpyHostToDevice);
    cudaMalloc(&DN, name_len);
    cudaMemcpy(DN, &name[0], name_len, cudaMemcpyHostToDevice);
    cudaMalloc(&index, number_of_contact*sizeof(int));
    cudaMemcpy(index, &contact_index[0], number_of_contact*sizeof(int), cudaMemcpyHostToDevice);


    cudaEvent_t start, end;
    cudaEventCreate(&start);
    cudaEventCreate(&end);
    cudaEventRecord(start);

    //searchContact<<<1, no_of_core>>>(DC, DN, index, contact_len, name_len, number_of_contact, no_of_core);
    searchContact2<<<1, no_of_core>>>(DC, DN, index, contact_len, name_len, no_of_core, number_of_contact);

    cudaDeviceSynchronize();
    cudaEventRecord(end);
    cudaEventSynchronize(end);
    float miliseconds = 0;
    cudaEventElapsedTime(&miliseconds, start, end);
    cout << "Time Taken : " << miliseconds << endl;

    vector<int>res(number_of_contact);
    cudaMemcpy(&res[0], index, number_of_contact*sizeof(int), cudaMemcpyDeviceToHost);

    for(int i=0; i<number_of_contact; i++)
    {
        if(res[i]==-1) cout << original_list[i] << endl;
    }
    cout << res.size() << endl;

}