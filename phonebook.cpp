#include<bits/stdc++.h>
#include<mpi.h>
using namespace std;

void send_a_number(int value, int to)
{
    MPI_Send(&value, 1, MPI_INT, to, 0, MPI_COMM_WORLD);
}

int get_a_number(int from)
{
    int value;
    MPI_Recv(&value, 1, MPI_INT, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return value;
}

void send_string(string str, int to)
{
    int length = str.size()+1;
    send_a_number(length, to);
    MPI_Send(str.c_str(), length, MPI_CHAR, to, 0 ,MPI_COMM_WORLD);
}

string get_string(int from)
{
    int length = get_a_number(from);
    char *str;
    str = new char[length];
    MPI_Recv(str, length, MPI_CHAR, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return string(str);
}

void check(string text, string pattern)
{
    int text_len = text.size();
    int pattern_len = pattern.size();
    int j = 0;
    set<string>st;
    for(int i =0; i<text_len; i++)
    {
        if(tolower(text[i])==tolower(pattern[j])) j++;
        else j = 0;
        if(j==pattern_len)
        {
            st.insert(text);
            break;
        }
    }
    for(auto str:st) cout << str << endl;
}

string create_string(vector<string> mainstring, int start, int end)
{
    string ans = "";
    for (int i=start; i<end; i++)
    {
        ans += mainstring[i]+"#";
    }
    return ans;
}

vector<string> split_string(string str)
{
    vector<string>ans;
    string temp;
    for(auto ch:str)
    {
        if(ch!='#') temp+=ch;
        else
        {
            ans.push_back(temp);
            temp = "";
        }
    }
    return ans;
}



int main(int argc, char * argv[])
{
    MPI_Init(&argc, &argv);
    int number_of_thread;
    int thread_id;
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_thread);
    MPI_Comm_rank(MPI_COMM_WORLD, &thread_id);

    if(thread_id==0)
    {
        string name = argv[1];
        vector<string>contact_list;
        vector<string> file_names(argv+2, argv+argc);
        for(auto file_name : file_names)
        {
            ifstream file(file_name);
            string line;
            while(getline(file, line))
            {
                contact_list.push_back(line);
            }
            file.close();
        }

        int number_of_contacts = contact_list.size();

        clock_t start_time, end_time;
        start_time = clock();
        
        for(int i=1; i<number_of_thread; i++)
        {
            send_string(name, i);

            int start_index = i* (number_of_contacts/number_of_thread);
            int end_index = (i+1) * (number_of_contacts/number_of_thread);
            if(i==number_of_thread-1) end_index = number_of_contacts;

            string splitedString = create_string(contact_list, start_index, end_index);
            send_string(splitedString, i);
        }

        contact_list.resize(number_of_contacts/number_of_thread);

        int n = contact_list.size();
        for(int i=0; i<n; i++)
        {
            check(contact_list[i], name);
        }


        end_time = clock();
        sleep(5);
        cout << "Total time needed : " << double(end_time-start_time)/CLOCKS_PER_SEC << endl;
    }
    else
    {
        string name = get_string(0);
        string splitted_contacts = get_string(0);
        auto contact_list = split_string(splitted_contacts);
        for(int i=0; i<contact_list.size(); i++)
        {
            check(contact_list[i], name);
        }
    }

    MPI_Finalize();
    return 0;
    
}