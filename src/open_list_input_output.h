//
// Created by cheyulin on 11/21/15.
//

#ifndef SRC_OPEN_LIST_INPUT_OUTPUT_H
#define SRC_OPEN_LIST_INPUT_OUTPUT_H

#endif //SRC_OPEN_LIST_INPUT_OUTPUT_H

#include "open_list_solver.h"
namespace election{
    class IOProcessor{
    private:
        vector<CandidateInfo> GetPartyCandidates(string &line);

    public:
        Solver *GetSolver(string in_file_name);
    };
}
