#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

int main() {
    int n;
    cin >> n;
    vector<int> v(n);
    for (int& x : v) {
        cin >> x;
    }

    sort(begin(v), end(v),
         [](int l, int r) {
             return abs(l) < abs(r);
         }
    );

    for (int x : v) {
        cout << x << " ";
    }

    return 0;
}
