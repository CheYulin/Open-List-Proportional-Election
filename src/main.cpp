
#include "open_list_solver.h"


vector<CandidateInfo> GetPartyCandidates(string &line) {
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

Solver *GetSolver(string in_file_name) {
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
            cout << "seats num:" << seats_num << endl;
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

void TestCandidatesInfo(Party *party) {
    map<CandidateId, CandidateInfo> candidates_info = party->getCandidates_info_();
    for (auto candidate_info : candidates_info) {
        cout << candidate_info.first << "," << candidate_info.second.candidate_name_ << "," <<
        candidate_info.second.candidate_vote_count_ << endl;
    }
    cout << endl;
}

void TestPartitionDetail(Party *party) {
    TestCandidatesInfo(party);
    for (ComparePartitionGroupSet sets: party->getGroups_info_with_different_size_()) {
        for (Group data: sets) {
            cout << "(";
            for (int integer : data.candidates_) {
                cout << integer;
            }
            cout << "," << data.group_vote_count_ << ")" << "  ";
        }
        cout << endl;
    }
}

void TestGroupsInit(Solver *solver) {
    TestPartitionDetail(solver->getFirst_party_());
    cout << endl;
    TestPartitionDetail(solver->getSecond_party_());
}

int main() {
    string in_file_name = "/home/cheyulin/gitrepos/Open-List-Proportional-Representation/input/input_seatnum_1_group1_5_group2_4.txt";
    Solver *my_solver = GetSolver(in_file_name);
    TestGroupsInit(my_solver);
    delete my_solver;
    return 0;
}