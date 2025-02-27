#include <iostream>
#include <array>
#include <vector>

using namespace std;

class Solver {
public:
    array<int, 26> column;

    array<int, 26> decrypt_map;
    array<bool, 10> val_used;

    void reset() {
        column.fill(0);
        decrypt_map.fill(-1);
        val_used.fill(false);
    }
    
}


int main() {
    



    array<int, 26> column;
    column.fill(0);
    column[2] = 2;
    column[1] = 1;

    array<int, 26> decrypt_map;
    decrypt_map.fill(-1);

    array<bool, 10> val_used;
    val_used.fill(false);

    vector<int> unknown_symbols;
    int known_sum = 0;

    for (int s = 0; s < 26; s++) {
        if (column[s] > 0) {
            if (decrypt_map[s] == -1) {
                unknown_symbols.push_back(s);
            } else {
                known_sum += decrypt_map[s] * column[s];
            }
        }
    }

    cout << "Known Sum: " << known_sum << endl;
    for (int s : unknown_symbols) {cout << s << " ";}
    cout << endl;
}