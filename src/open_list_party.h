//
// Created by cheyulin on 11/19/15.
//
#ifndef SRC_OPEN_LIST_DATA_H
#define SRC_OPEN_LIST_DATA_H

#endif //SRC_OPEN_LIST_DATA_H

#include <memory>
#include <set>
#include <map>
#include <vector>
#include <queue>
#include <cmath>
#include "open_list_solver.h"

using namespace std;
using namespace election;
namespace election {
    typedef int CandidateId;
    typedef int CandidateVoteCount;
    typedef string CandidateName;
    typedef double StrategyPayOff;

    struct Group {
        vector<CandidateId> candidates_;
        int group_vote_count_;
    };

    struct GroupCmp {
        bool operator()(Group group_left, Group group_right) {
            return group_left.group_vote_count_ < group_right.group_vote_count_;
        };
    };

    struct CandidateInfo{
        CandidateName candidate_name_;
        CandidateVoteCount candidate_vote_count_;
    };

    class Party {
    private:
        int sum_votes_;
        map<CandidateId, CandidateInfo> candidates_info_;
        vector<vector<Group>> groups_info_with_different_size_;

        void InitGroupsAlternativesInfo();

    public:
        Party(vector<CandidateInfo> candidates_info, int seats_num);
        int GetSumVotes();
    };



}
