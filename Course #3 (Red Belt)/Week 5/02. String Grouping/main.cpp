#include <algorithm>
#include <deque>
#include <string>
#include <vector>
#include <map>

using namespace std;

template <typename String>
using Group = vector<String>;

template <typename String>
using Char = typename String::value_type;

template <typename String>
using Key = String;


template <typename String>
Key<String> ComputeStringKey(const String& string) {
    String chars = string;
    sort(begin(chars), end(chars));
    chars.erase(unique(begin(chars), end(chars)), end(chars));
    return chars;
}


template <typename String>
vector<Group<String>> GroupHeavyStrings(vector<String> strings) {
    map<Key<String>, Group<String>> groups_map;
    for (String& string : strings) {
        groups_map[ComputeStringKey(string)].push_back(move(string));
    }
    vector<Group<String>> groups;
    for (auto& [key, group] : groups_map) {
        groups.push_back(move(group));
    }
    return groups;
}
