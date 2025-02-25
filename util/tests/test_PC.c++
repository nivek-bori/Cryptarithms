// Process Colum Test
// Unit Test: Given column (Symbol to Frequency), assert known_sum & unknown are correctly processed

#include <iostream>
#include <array>

using namespace std;

array<int, 26> decrypt_map; // Symbol to Value - If (val == -1) -> Symbol not assigned
array<bool, 10> val_used;

// Return is passed through referenced - known_sum & unknown
void processCol(array<int, 26> &col, int &known_sum, array<bool, 26> &unknown) {
    known_sum = 0; // Referenced return
    unknown.fill(false); // // Referenced return

    for (int s = 0; s < 26; s++) {
        if (col.at(s) != -1) {
            if (decrypt_map.at(s) != -1) {
                known_sum += decrypt_map.at(s) * col.at(s); // Value of (s) * Frequency of (s)
            } else {
                unknown[s] = true;
            }
        }
    }
}

int main() {
    // Data Structure Initialization
    decrypt_map.fill(-1);
    val_used.fill(false);

    int known_sum = 0;
    array<bool, 26> unknown;
    unknown.fill(false);

    // Unit Test Data
    array<int, 26> col;
    col.fill(-1);

    int unit_test_num = 6;
    switch (unit_test_num) {
        case 0: // 1-2 Spead
            col[1] = 1;
            col[3] = 2;
            break; // Assert: known_sum == 0 & unknown = {1, 3}
        case 1: // 3 Unknown
            col[1] = 1;
            col[3] = 1;
            col[5] = 1;
            break; // Assert: known_sum == 0 & unknown = {1, 3, 5}
        case 2: // 2 Maped, 1 Unknown
            decrypt_map[1] = 3; col[1] = 1;
            decrypt_map[5] = 2; col[5] = 1;
            col[6] = 1;
            break; // Assert: known_sum == 5 & unknown = {6}
        case 3: // 1 Mapped, 2 Unknown, 1 Extra
            decrypt_map[1] = 3; col[1] = 1;
            decrypt_map[8] = 2;
            col[5] = 1;
            col[6] = 1;
            break; // Assert: known_sum == 3 & unknown = {5, 6}
        case 4: // 3 Mapped
            decrypt_map[1] = 3; col[1] = 1;
            decrypt_map[5] = 4; col[5] = 1;
            decrypt_map[6] = 2; col[6] = 1;
            break; // Assert: known_sum == 9 & unknown = {}
        case 5: // Mapped 1-2 Spread
            decrypt_map[1] = 3; col[1] = 1;
            decrypt_map[5] = 4; col[5] = 2;
            break; // Assert: known_sum == 11 & unknown = {}
        case 6: // 3 Mapped, 1 Extra
            decrypt_map[1] = 3; col[1] = 1;
            decrypt_map[5] = 4; col[5] = 1;
            decrypt_map[6] = 6; col[6] = 1;
            decrypt_map[7] = 5;
            break; // Assert: known_sum == 13 & unknown = {}
    }

    // Testing
    processCol(col, known_sum, unknown);

    // Logging
    cout << "Known Sum: " << known_sum << endl;
    cout << "Unknown: ";
    for (int s = 0; s < 26; s++) {
        if (unknown.at(s)) {cout << s << ", ";}
    }
    cout << endl;

    cout << "TASK END" << endl;
    return 0;
}