//
// Created by cheyulin on 11/19/15.
//
#ifndef SRC_OPEN_LIST_DATA_H
#define SRC_OPEN_LIST_DATA_H

#endif //SRC_OPEN_LIST_DATA_H

#include "open_list_includes.h"

namespace election {
    typedef int CandidateId;
    typedef int CandidateVoteCount;
    typedef string CandidateName;

    struct Group {
        vector<CandidateId> candidates_;
        int group_vote_count_;
    };

    struct GroupComparePartition {
        bool operator()(const Group &left_group, const Group &right_group) const;
    };

    struct GroupCompareVote {
        bool operator()(const Group &left_group, const Group &right_group) const;
    };

    typedef set<Group, GroupComparePartition> ComparePartitionGroupSet;

    struct CandidateInfo {
        CandidateName candidate_name_;
        CandidateVoteCount candidate_vote_count_;

        CandidateInfo();

        CandidateInfo(CandidateName candidate_name, CandidateVoteCount candidate_vote_count);
    };

    class Party {
    private:
        int sum_votes_;
        map<CandidateId, CandidateInfo> candidates_info_;
        vector<ComparePartitionGroupSet> groups_info_with_different_size_;

        void InitGroupsAlternativesInfo();

    public:
        Party(vector<CandidateInfo> candidates_info, int seats_num);

        int GetSumVotes();

        //Auto Generated
        const vector<ComparePartitionGroupSet> &getGroups_info_with_different_size_() const {
            return groups_info_with_different_size_;
        }

        const map<CandidateId, CandidateInfo> &getCandidates_info_() const {
            return candidates_info_;
        }


    };


}
