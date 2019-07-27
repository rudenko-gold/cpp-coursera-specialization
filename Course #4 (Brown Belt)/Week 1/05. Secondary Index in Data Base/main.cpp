#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

struct Record {
    string id;
    string title;
    string user;
    int timestamp;
    int karma;

    bool operator== (const Record& other) const {
        return id == other.id;
    }
};

class Database {
public:
    bool Put(const Record& record) {
        if (db.find(record.id) == db.end()) {
            db[record.id] = record;
            const Record& record_ref = db[record.id];
            Iterators& its = iterators[record_ref.id];

            its.by_user = by_user.insert({record_ref.user, record_ref});
            its.by_timestamp = by_timestamp.insert({record_ref.timestamp, record_ref});
            its.by_karma = by_karma.insert({record_ref.karma, record_ref});

            return true;
        } else {
            return false;
        }
    }

    const Record* GetById(const string& id) const {
        if (db.find(id) == db.end()) {
            return nullptr;
        } else {
            return &db.at(id);
        }
    }

    bool Erase(const string& id) {
        if (db.find(id) == db.end()) {
            return false;
        } else {
            db.erase(id);

            Iterators& its = iterators[id];

            by_user.erase(its.by_user);
            by_timestamp.erase(its.by_timestamp);
            by_karma.erase(its.by_karma);

            iterators.erase(id);

            return true;
        }
    }

    template <typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const {
        if (low > high) {
            return;
        }

        auto range_begin = by_timestamp.lower_bound(low);
        auto range_end = by_timestamp.upper_bound(high);

        for (auto it = range_begin; it != range_end; ++it) {
            if (!callback(it->second)) {
                return;
            }
        }
    }

    template <typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const {
        if (low > high) {
            return;
        }

        auto range_begin = by_karma.lower_bound(low);
        auto range_end = by_karma.upper_bound(high);

        for (auto it = range_begin; it != range_end; ++it) {
            if (!callback(it->second)) {
                return;
            }
        }
    }

    template <typename Callback>
    void AllByUser(const string& user, Callback callback) const {
        auto range_begin = by_user.lower_bound(user);
        auto range_end = by_user.upper_bound(user);

        for (auto it = range_begin; it != range_end; ++it) {
            if (!callback(it->second)) {
                return;
            }
        }
    }

private:
    struct Iterators {
        multimap<string, const Record&>::iterator by_user;
        multimap<int, const Record&>::iterator by_timestamp;
        multimap<int, const Record&>::iterator by_karma;
    };

    unordered_map<string, Record> db;
    unordered_map<string, Iterators> iterators;

    multimap<string, const Record&> by_user;
    multimap<int, const Record&> by_timestamp;
    multimap<int, const Record&> by_karma;
};

void TestRangeBoundaries() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
    db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestSameUser() {
    Database db;
    db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
    db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

    int count = 0;
    db.AllByUser("master", [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestReplacement() {
    const string final_body = "Feeling sad";

    Database db;
    db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
    db.Erase("id");
    db.Put({"id", final_body, "not-master", 1536107260, -10});

    auto record = db.GetById("id");
    ASSERT(record != nullptr);
    ASSERT_EQUAL(final_body, record->title);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
    return 0;
}
