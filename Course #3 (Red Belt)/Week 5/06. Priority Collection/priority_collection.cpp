#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <utility>
#include <vector>

using namespace std;

template <typename T>
class PriorityCollection {
public:
    using Id = int;

    Id Add(T object) {
        objects.emplace(lastId, move(object));
        data[0].insert(lastId);
        priors[lastId] = 0;
        return lastId++;
    }

    template <typename ObjInputIt, typename IdOutputIt>
    void Add(ObjInputIt range_begin, ObjInputIt range_end,
             IdOutputIt ids_begin) {
            while(range_begin != range_end) {
                objects.emplace(lastId, move(*range_begin));

                data[0].insert(lastId);
                priors[lastId] = 0;
                *ids_begin = lastId;
                lastId++;

                ids_begin++;
                range_begin++;
            }
    }

    bool IsValid(Id id) const {
        return objects.find(id) != objects.end();
    }

    const T& Get(Id id) const {
        return objects.at(id);
    }

    void Promote(Id id) {
        int currPrior = priors[id];
        data[currPrior].erase(id);
        if (data[currPrior].size() == 0) {
            data.erase(currPrior);
        }
        data[currPrior + 1].insert(id);
        priors[id]++;
    }

    pair<const T&, int> GetMax() const {
        auto it = data.end();
        it--;

        Id id = *((*it).second.rbegin());
        int prior = (*it).first;

        return { move(objects.at(id)), prior };
    }

    pair<T, int> PopMax() {
        auto it = data.end();
        it--;

        Id id = *((*it).second.rbegin());
        int prior = (*it).first;
        pair p = { move(objects.at(id)), prior };

        objects.erase(id);
        priors.erase(id);

        (*it).second.erase(id);
        if ((*it).second.size() == 0) {
            data.erase(it);
        }


        return p;
    }

private:
    Id lastId = 0;
    map<Id, T> objects;
    map<Id, int> priors;
    map<int, set<Id>> data;
};


class StringNonCopyable : public string {
public:
    using string::string;
    StringNonCopyable(const StringNonCopyable&) = delete;
    StringNonCopyable(StringNonCopyable&&) = default;
    StringNonCopyable& operator=(const StringNonCopyable&) = delete;
    StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
    PriorityCollection<StringNonCopyable> strings;
    vector<StringNonCopyable> toAdd(3);
    toAdd[0] =  "white";
    toAdd[1] = "yellow";
    toAdd[2] = "red" ;
    vector<int> ids(3);

    strings.Add(toAdd.begin(), toAdd.end(), ids.begin());

    const auto white_id = ids[0];
    const auto yellow_id = ids[1];
    const auto red_id = ids[2];

    strings.Promote(yellow_id);
    for (int i = 0; i < 2; ++i) {
        strings.Promote(red_id);
    }
    strings.Promote(yellow_id);
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "red");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "yellow");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "white");
        ASSERT_EQUAL(item.second, 0);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestNoCopy);
    return 0;
}
