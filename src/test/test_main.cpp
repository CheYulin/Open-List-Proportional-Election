//
// Created by cheyulin on 11/19/15.
//
#include <iostream>
#include <map>
using namespace std;
int main(){
    multimap<int,int> my_map;
    my_map.insert(make_pair(1,2));
    my_map.insert(make_pair(1,3));
    my_map.insert(make_pair(2,4));
    for(auto iter= my_map.begin(); iter!= my_map.end(); iter++){
        cout<<iter->first<<","<<iter->second<<endl;
    }
}