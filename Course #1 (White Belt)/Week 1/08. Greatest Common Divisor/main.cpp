#include <iostream>

using namespace std;

int main() {
    int a, b;
    cin >> a >> b;

    while (a > 0 && b > 0) {
        a %= b;
        swap(a, b);
    }

    cout << a + b;

    return 0;
}
