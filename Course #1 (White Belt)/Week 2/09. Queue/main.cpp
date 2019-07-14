#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int q;
    cin >> q;
    vector<bool> is_nervous;

    for (int i = 0; i < q; ++i) {
        string operation_code;
        cin >> operation_code;

        if (operation_code == "WORRY_COUNT") {
            cout << count(begin(is_nervous), end(is_nervous), true) << endl;
        } else {
            if (operation_code == "WORRY" || operation_code == "QUIET") {

                int person_index;
                cin >> person_index;
                
                is_nervous[person_index] = (operation_code == "WORRY");

            } else if (operation_code == "COME") {

                int person_count;
                cin >> person_count;
                
                is_nervous.resize(is_nervous.size() + person_count, false);

            }
        }
    }

    return 0;
}
