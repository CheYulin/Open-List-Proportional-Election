//
// Created by yche on 11/20/2015.
//

#include <iomanip>
#include "../../open_list_includes.h"

struct Group {
    vector<int> candidates_;
    int vote;

    bool operator==(const Group &other_group) const {
        if (other_group.candidates_.size() != this->candidates_.size())
            return false;
        for (int i = 0; i < other_group.candidates_.size(); i++) {
            if (other_group.candidates_[i] != this->candidates_[i])
                return false;
        }
        cout << "shit:";
        for (int integer : other_group.candidates_) {
            cout << integer << "shit";
        }
        return true;
    }

    bool operator<(const Group &right_group) const {
        int left_collection_size = this->candidates_.size();
        int right_collection_size = right_group.candidates_.size();
        int max_size = left_collection_size < right_collection_size ? left_collection_size : right_collection_size;
        for (int i = 0; i < max_size; i++) {
            if (this->candidates_[i] < right_group.candidates_[i]) {
                return true;
            }
            else if (this->candidates_[i] > right_group.candidates_[i]) {
                return false;
            }
        }
        if (max_size < right_collection_size) {
            return true;
        }
        else
            return false;
    }

};

struct GroupCmp {
    bool operator()(const Group &left_group, const Group &right_group) const {
        int left_collection_size = left_group.candidates_.size();
        int right_collection_size = right_group.candidates_.size();
        int max_size = left_collection_size < right_collection_size ? left_collection_size : right_collection_size;
        for (int i = 0; i < max_size; i++) {
            if (left_group.candidates_[i] < right_group.candidates_[i]) {
                return true;
            }
            else if (left_group.candidates_[i] > right_group.candidates_[i]) {
                return false;
            }
        }
        if (max_size < right_collection_size) {
            return true;
        }
        else
            return false;
    }
};

void TestConstrctCombination() {
    map<int, int> my_map;
    my_map.insert(make_pair(1, 2));
    my_map.insert(make_pair(2, 3));
    my_map.insert(make_pair(3, 2));
    my_map.insert(make_pair(4, 4));
    my_map.insert(make_pair(5, 7));
    int map_size = my_map.size();

    vector<vector<Group>> my_vector_vector = vector<vector<Group>>(map_size);
    vector<Group> first_vector = vector<Group>();
    for (auto pair : my_map) {
        Group data = Group();
        data.candidates_.push_back(pair.first);
        data.vote = pair.second;
        first_vector.push_back(data);
    }
    my_vector_vector.push_back(first_vector);

    vector<Group> former_vector = first_vector;
    vector<Group> latter_vector = vector<Group>();
    for (int i = 1; i < map_size; i++) {
        for (Group former_data: former_vector) {
            int last_value = former_data.candidates_[former_data.candidates_.size() - 1];
            int max_value = first_vector[first_vector.size() - 1].candidates_[0];
            for (int j = last_value + 1; j <= max_value; j++) {
                Group new_data = former_data;
                new_data.candidates_.push_back(j);
                new_data.vote += first_vector[j - 1].vote;
                latter_vector.push_back(new_data);
            }
        }
        my_vector_vector.push_back(latter_vector);
        former_vector = latter_vector;
        latter_vector = vector<Group>();
    }

    for (vector<Group> vectors: my_vector_vector) {
        for (Group data: vectors) {
            cout << "(";
            for (int integer : data.candidates_) {
                cout << integer;
            }
            cout << "," << data.vote << ")" << "  ";
        }
        cout << endl;
    }
}

void TestCOnstructCOmbinationWithSet() {
    map<int, int> my_map;
    my_map.insert(make_pair(1, 2));
    my_map.insert(make_pair(2, 3));
    my_map.insert(make_pair(3, 2));
    my_map.insert(make_pair(4, 4));
    my_map.insert(make_pair(5, 7));
    my_map.insert(make_pair(6, 9));
    int map_size = my_map.size();

    vector<set<Group, GroupCmp>> groups_info_with_different_size_ = vector<set<Group, GroupCmp>>();
    set<Group, GroupCmp> first_groups_with_size_one = set<Group, GroupCmp>();
    vector<Group> first_groups_with_size_one_info = vector<Group>();
    for (auto pair : my_map) {
        Group data = Group();
        data.candidates_.push_back(pair.first);
        data.vote = pair.second;
        first_groups_with_size_one.insert(data);
        first_groups_with_size_one_info.push_back(data);
    }

    groups_info_with_different_size_.push_back(first_groups_with_size_one);

    set<Group, GroupCmp> former_groups_with_same_size = first_groups_with_size_one;
    set<Group, GroupCmp> latter_groups_with_same_size = set<Group, GroupCmp>();
    int max_value = first_groups_with_size_one_info[first_groups_with_size_one_info.size() - 1].candidates_[0];
    for (int i = 1; i < map_size; i++) {
        for (Group former_data: former_groups_with_same_size) {
            int last_value = former_data.candidates_[former_data.candidates_.size() - 1];
            for (int j = last_value + 1; j <= max_value; j++) {
                Group new_data = former_data;
                new_data.candidates_.push_back(j);
                new_data.vote += first_groups_with_size_one_info[j - 1].vote;
                latter_groups_with_same_size.insert(new_data);
            }
        }
        groups_info_with_different_size_.push_back(latter_groups_with_same_size);
        former_groups_with_same_size = latter_groups_with_same_size;
        latter_groups_with_same_size = set<Group, GroupCmp>();
    }

    for (set<Group, GroupCmp> sets: groups_info_with_different_size_) {
        for (Group data: sets) {
            cout << "(";
            for (int integer : data.candidates_) {
                cout << integer;
            }
            cout << "," << data.vote << ")" << "  ";
        }
        cout << endl;
    }
    cout << endl;
    Group test_group;
    test_group.candidates_.push_back(1);
    test_group.candidates_.push_back(2);
    test_group.candidates_.push_back(4);
    for (set<Group, GroupCmp> sets: groups_info_with_different_size_) {
        if (sets.find(test_group) != sets.end()) {
            cout << "(";
            for (int integer :sets.find(test_group)->candidates_)
                cout << integer;
            cout << "," << sets.find(test_group)->vote << ")" << endl;
        }
    }
}

int main() {
//    vector<Group> *my_vector = new vector<Group>(10);
//    delete my_vector;
//    std::ios_base::sync_with_stdio(false);
//    cout << setiosflags(ios::fixed) << setprecision(2) << (double) 100 << "~~~~" << 100.324324 << endl;
//    cout << "haha" << 43243.432432 << endl;
    cout <<  sizeof(char) << endl;
    cout << (2 >> 2);
}

