#include <iostream>
#include <fstream>
#include <regex>
#include "open_list_party.h"
using namespace std;
using namespace election;
int main() {
    string in_file_name = "/home/cheyulin/gitrepos/Open-List-Proportional-Representation/input/input_seatnum_1_group1_5_group2_4.txt";
    ifstream infile(in_file_name.c_str(), ios::in);
    string line;
    int line_num = 0;
    int seats_num =0;
    vector<CandidateInfo> candidates_info;
    const regex split_pattern(").(");
    while(!infile.eof())
    {
        line_num++;
        getline(infile,line);
        if(line_num == 1){
            seats_num = atoi(line.c_str());
        }
        else if(line_num ==2){
            vector<string> str_vector;
            sregex_iterator iter(line.begin(), line.end(), split_pattern, -1);
            sregex_iterator end;
            while(iter!=end){
                str_vector.push_back(*iter++);
            }
            cout << endl;
        }
        else if(line_num ==3){

        }
        else{
            break;
        }
//        cout << line << endl;
    }
    cout <<"seats num:" <<seats_num<<endl;
    return 0;
}