//
// Created by cheyulin on 11/21/15.
//

#include "open_list_input_output.h"

vector<CandidateInfo> IOProcessor::GetPartyCandidates(string &line) {
//    regex split_pattern("\\)|,|\\(");
//    line = regex_replace(line, split_pattern, " ");
    for(int i=0; i<line.size();i++){
       if(line[i] == '(' || line[i] == ')' || line[i] == ','){
            line[i]= ' ';
       }
    }
    stringstream string_stream;
    string_stream.clear();
    string_stream.str(line);
    vector<CandidateInfo> candidates_info;
    while (true) {
        string candidate_name;
        int candidate_vote;
        string_stream >> candidate_name;
        if (string_stream.eof()) {
            break;
        }
        string_stream >> candidate_vote;
        candidates_info.push_back(CandidateInfo(candidate_name, candidate_vote));
    }
    return candidates_info;
}

SolverInfo IOProcessor::GetSolverInfo(string in_file_name) {
    ifstream infile(in_file_name.c_str(), ios::in);
    string line;
    int line_num = 0;

    SolverInfo solver_info;
    while (!infile.eof()) {
        line_num++;
        getline(infile, line);
        if (line_num == 1) {
            solver_info.seat_num = atoi(line.c_str());
        }
        else if (line_num == 2) {
            vector<CandidateInfo> candidates_info = GetPartyCandidates(line);
            solver_info.first_party_ = new Party(candidates_info);
        }
        else if (line_num == 3) {
            vector<CandidateInfo> candidates_info = GetPartyCandidates(line);
            solver_info.second_party_ = new Party(candidates_info);
        }
        else {
            break;
        }
    }

    return solver_info;
}
