//
// Created by cheyulin on 11/23/15.
//

#include "open_list_party_basic_info.h"
using namespace election;

//Candidate Related
CandidateInfo::CandidateInfo() {
}

CandidateInfo::CandidateInfo(CandidateName candidate_name, CandidateVoteCount candidate_vote_count) : candidate_name_(
        candidate_name), candidate_vote_count_(candidate_vote_count) {
}


//CandidateListInfo Related
string CandidateListInfo::GetCandidatesAsString() const {
    stringstream string_builder;
    for (CandidateId candidate_id: *candidates_) {
        string_builder << candidate_id;
    }
    return string_builder.str();
}

CandidateListInfo::CandidateListInfo() {
    candidates_ = new vector<CandidateId>();
}

CandidateListInfo::CandidateListInfo(const CandidateListInfo &candidate_list_info) : group_vote_count_(
        candidate_list_info.group_vote_count_) {
    candidates_ = new vector<CandidateId>();
    for (auto candidate_id : *candidate_list_info.candidates_) {
        candidates_->push_back(candidate_id);
    }
}

bool GroupCandidatesCompare::operator()(const CandidateListInfo *left_group,
                                        const CandidateListInfo *right_group) const {
    int left_collection_size = left_group->candidates_->size();
    int right_collection_size = right_group->candidates_->size();
    int max_size = left_collection_size < right_collection_size ? left_collection_size : right_collection_size;
    for (int i = 0; i < max_size; i++) {
        if ((*left_group->candidates_)[i] < (*right_group->candidates_)[i]) {
            return true;
        }
        else if ((*left_group->candidates_)[i] > (*right_group->candidates_)[i]) {
            return false;
        }
    }
    if (max_size < right_collection_size) {
        return true;
    }
    else
        return false;
}

bool GroupVoteCompare::operator()(const CandidateListInfo *left_group, const CandidateListInfo *right_group) {
    return left_group->group_vote_count_ < right_group->group_vote_count_;
}

