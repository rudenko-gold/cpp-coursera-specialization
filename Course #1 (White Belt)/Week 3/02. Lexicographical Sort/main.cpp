#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int n;
    cin >> n;
    vector<string> v(n);
    for (string& s : v) {
        cin >> s;
    }

    sort(begin(v), end(v),
         [](const string& l, const string& r) {
             return lexicographical_compare(
                     begin(l), end(l),
                     begin(r), end(r),
                     [](char cl, char cr) { return tolower(cl) < tolower(cr); }
             );
         }
    );

    for (const string& s : v) {
        cout << s << ' ';
    }

    return 0;
}
