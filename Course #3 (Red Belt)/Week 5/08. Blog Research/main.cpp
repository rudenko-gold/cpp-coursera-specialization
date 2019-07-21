#include "test_runner.h"
#include "profile.h"

#include <map>
#include <string>
#include <string_view>
#include <future>

using namespace std;

struct Stats {
    map<string, int> word_frequences;

    void operator += (const Stats& other) {
        for (const auto& p : other.word_frequences) {
            word_frequences[p.first] += p.second;
        }
    }
};

Stats ExploreKeyWordsSingleThread(
        const set<string>& key_words, const vector<string>& words, int firstWordInd, int page_size
) {
    Stats result;

    for (size_t i = 0; i < page_size && firstWordInd + i < words.size(); ++i) {
        if (key_words.find(words[firstWordInd + i]) != key_words.end()) {
            result.word_frequences[words[firstWordInd + i]]++;
        }
    }

    return result;
}

Stats ExploreKeyWords(const set<string>& key_words, istream& input) {
    vector<string> words;
    string word;
    while (input >> word) {
        words.push_back(word);
    }

    int page_size = (words.size() % 4 == 0) ? (words.size() / 4) : (words.size() / 4 + 1);
    vector<future<Stats>> futures;
    for (int i = 0; i < words.size(); i += page_size) {
        futures.push_back(async([&words, page_size, &key_words, i] () {
            return ExploreKeyWordsSingleThread(key_words, words, i, page_size);
        }));
    }

    Stats result;

    for (auto& f : futures) {
        result += f.get();
    }

    return result;
}

void TestBasic() {
    const set<string> key_words = {"yangle", "rocks", "sucks", "all"};

    stringstream ss;
    ss << "this new yangle service really rocks\n";
    ss << "It sucks when yangle isn't available\n";
    ss << "10 reasons why yangle is the best IT company\n";
    ss << "yangle rocks others suck\n";
    ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

    const auto stats = ExploreKeyWords(key_words, ss);
    const map<string, int> expected = {
            {"yangle", 6},
            {"rocks", 2},
            {"sucks", 1}
    };
    ASSERT_EQUAL(stats.word_frequences, expected);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestBasic);
}
