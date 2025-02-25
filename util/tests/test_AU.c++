// Assign Unknowns Test
// Unit Test: Given x amount of unknown symbols and y amount of valid values --> assert number of full assignments == (y)! / (y - x)!
// Note: Because values cannot repeat, this is not a permutation problem - it is a combination problem

#include <iostream>
#include <array>
using namespace std;

// Data Structure Declaration
int count;
array<int, 26> decrypt_map; // Symbol to Value - If (val == -1) -> Symbol not assigned
array<bool, 10> val_used;

void assignUnknown(int s, int &known_sum, array<bool, 26> &unknown) {
    for (; s < 26; s++) {
        if (unknown.at(s)) {
            for (int val = 0; val < 10; val++) {
                if (!val_used.at(val)) {
                    decrypt_map[s] = val;
                    val_used[val] = true;
                    known_sum += val;

                    assignUnknown(s + 1, known_sum, unknown);

                    decrypt_map[s] = -1;
                    val_used[val] = false;
                    known_sum -= val;
                }
            }

            return;
        }
    }

    // Assertion: All unknown are assigned

    ::count++;
    for (int s = 0; s < 26; s++) {
        if (decrypt_map.at(s) != -1) {
            cout << decrypt_map.at(s) << ", ";
        }
    }
    cout << endl;
}

int main() {
    // Data Structure Initialization
    ::count = 0;
    decrypt_map.fill(-1);
    val_used.fill(false);

    // Unit Test Data
    int checkI = 0;
    int known_sum = 0;
    array<bool, 26> unknown;
    unknown.fill(false);

    // Unit Test: Add or remove lines for unit tests
    //    Logic: Given x unknown symbols and y valid values, assert count == (y)! / (y - x)!
    unknown[4] = true;
    unknown[2] = true;
    unknown[9] = true;
    unknown[25] = true;
    unknown[23] = true;

    assignUnknown(0, known_sum, unknown);
    cout << "Total Assignmnets: " << ::count << endl;
}