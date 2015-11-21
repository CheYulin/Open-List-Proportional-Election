//
// Created by cheyulin on 11/21/15.
//

#include "open_list_input_output.h"

vector<CandidateInfo> IOProcessor::GetPartyCandidates(string &line) {
    regex split_pattern("\\)|,|\\(");
    line = regex_replace(line, split_pattern, " ");
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

Solver *IOProcessor::GetSolver(string in_file_name) {
    ifstream infile(in_file_name.c_str(), ios::in);
    string line;
    int line_num = 0;
    int seats_num = 0;
    Party *first_party;
    Party *second_party;

    while (!infile.eof()) {
        line_num++;
        getline(infile, line);
        if (line_num == 1) {
            seats_num = atoi(line.c_str());
        }
        else if (line_num == 2) {
            vector<CandidateInfo> candidates_info = GetPartyCandidates(line);
            first_party = new Party(candidates_info, seats_num);
        }
        else if (line_num == 3) {
            vector<CandidateInfo> candidates_info = GetPartyCandidates(line);
            second_party = new Party(candidates_info, seats_num);
        }
        else {
            break;
        }
    }
    Solver *my_solver = new Solver(first_party, second_party, seats_num);
    return my_solver;
}
