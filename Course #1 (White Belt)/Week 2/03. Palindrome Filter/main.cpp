#include <string>
#include <vector>

using namespace std;

bool IsPalindrom(string s) {
    for (size_t i = 0; i < s.size() / 2; ++i) {
        if (s[i] != s[s.size() - i - 1]) {
            return false;
        }
    }

    return true;
}

vector<string> PalindromFilter(vector<string> words, int min_length) {
    vector<string> result;
    
    for (auto s : words) {
        if (s.size() >= min_length && IsPalindrom(s)) {
            result.push_back(s);
        }
    }
    
    return result;
}
