//
// Created by cheyulin on 11/19/15.
//

#ifndef SRC_OPEN_LIST_DATA_H
#define SRC_OPEN_LIST_DATA_H

#endif //SRC_OPEN_LIST_DATA_H

#include <memory>
#include <map>
#include <vector>
using namespace std;

namespace election{
    typedef int CandidateId;
    typedef int CandidateVoteCount;
    typedef int GroupVoteCount;
    struct Group{
        vector<CandidateId> candidates;
        int group_vote_count;
    };

    struct Strategy{
        multimap<GroupVoteCount, shared_ptr<Group>> partition_info;
    };

    class Party{
    private:
        int max_partition_num;
        map<CandidateId,CandidateVoteCount> candidates_info;
        vector<Group> groups;
        void InitCombinationInfo(int seats_num);
    public:
        Party(map<CandidateId,CandidateVoteCount> candidate_info, int seats_num);
    };


    class Solver{
        
    };
}
