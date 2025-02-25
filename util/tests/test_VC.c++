// Verify Column Test
// Unit Test: TODO

#include <iostream>
#include <vector>
#include <array>
using namespace std;

vector<array<int, 26>> columns(1); // Index: Column, Value: <Index: Symbol, Value: Frequency>
vector<int> results(1); // Index: Column, Value: Symbol

array<int, 26> decrypt_map; // Symbol to Value - If (val == -1) -> Symbol not assigned
array<bool, 10> val_used;

int colI;
int carry;

bool testing_flag;

void logValues() {
    cout << "Testing Flag: " << testing_flag << endl;
    cout << "Pre Decrypt Map: ";
    for (int i = 0; i < 26; i++) {if (decrypt_map.at(i) != -1) {cout << "{" << i << " - " << decrypt_map.at(i) << "}, ";}}
    cout << endl;
    cout << "Pre Val Used: ";
    for (int i = 0; i < 10; i++) {if (val_used.at(i)) {cout << i << " ";}}
    cout << endl;
    cout << "Pre ColI: " << colI << endl;
    cout << "Pre Carry: " << carry << endl;
    cout << endl;
}

void verifyCol() {
    array<int, 26> &col = columns.at(colI); // Symbol to Frequency
    int resultS = results.at(colI);

    // Assert: All symbols in column are assigned
    for (int assertS = 0; assertS < 26; assertS++) {
        if (col.at(assertS) != -1 && decrypt_map.at(assertS) == -1) {
            cerr << "ERROR: Symbol in equation not assigned" << endl;
            logValues(); // TODO: REMOVE
            return;
        }
    }

    int sum = carry;
    for (int s = 0; s < 26; s++) {
        if (col.at(s) != -1) {
            sum += decrypt_map.at(s) * col.at(s);
        }
    }
    int remainder = sum % 10;

    if (decrypt_map.at(resultS) != -1) {
        if (decrypt_map.at(resultS) == remainder) {
            testing_flag = true; // TODO: REMOVE

            colI++;
            int old_carry = carry;
            carry = sum / 10;
            
            // solveColumn();
            logValues(); // TODO: REMOVE
            
            colI--;
            carry = old_carry;
        } else {
            testing_flag = false; // TODO: REMOVE
            logValues(); // TODO: REMOVE
        }
    } else if (!val_used.at(remainder)) {
        testing_flag = true; // TODO: REMOVE

        decrypt_map[resultS] = remainder;
        val_used[remainder] = true;
        
        colI++;
        int old_carry = carry;
        carry = sum / 10;
        
        // solveColumn();
        logValues(); // TODO: REMOVE

        decrypt_map[resultS] = -1;
        val_used[remainder] = false;

        colI--;
        carry = old_carry;
    } else {
        testing_flag = false; // TODO: REMOVE
        logValues(); // TODO: REMOVE
    }
}

int main() {
    array<int, 26> col;
    col.fill(-1);

    decrypt_map.fill(-1);
    val_used.fill(false);

    colI = 0;
    carry = 0;

    testing_flag = false;

    int unit_test_num = 1;
    switch (unit_test_num) {
        case 0: // Not all symbols assigned
            decrypt_map[1] = 2; col[1] = 1; val_used[2] = true;
            col[3] = 2;
            results[0] = 4;
            break; // Assert: Error & no continuation
        case 1: // Result symbol assigned correctly but not result value
            decrypt_map[1] = 2; col[1] = 1; val_used[2] = true;
            decrypt_map[3] = 5; col[3] = 2; val_used[3] = true;
            results[0] = 8;
            break; // Assert: Success
        case 2: // Result symbol assigned correctly but not result value
            decrypt_map[1] = 2; col[1] = 1; val_used[2] = true;
            decrypt_map[3] = 8; col[3] = 2; val_used[3] = true;
            results[0] = 8;
            break; // Assert: Success
        case 3: // Result symbol assigned incorrectly
            decrypt_map[1] = 2; col[1] = 1; val_used[2] = true;
            decrypt_map[3] = 3; col[3] = 1; val_used[3] = true;
            decrypt_map[4] = 4; results[0] = 4; val_used[4] = true;
            break; // Assert: No continuation
        case 4: // Result symbol not assigned
            decrypt_map[1] = 2; col[1] = 1; val_used[2] = true;
            decrypt_map[3] = 3; col[3] = 1; val_used[3] = true;
            decrypt_map[6] = 5; col[6] = 1; val_used[5] = true;
            results[0] = 4; 
            break; // Assert: Decrypt_map added {4 : 10}
        case 5: // Unused symbol assigned
            decrypt_map[1] = 2; col[1] = 1; val_used[2] = true;
            decrypt_map[3] = 3; col[3] = 1; val_used[3] = true;
            decrypt_map[6] = 7; col[6] = 1; val_used[7] = true;
            decrypt_map[7] = 9; val_used[9] = true;
            decrypt_map[10] = 1; val_used[10] = true;
            results[0] = 6; 
            break; // Assert: Decyrpt_map added {6: 12}
        case 6: // Incorrect result value
            decrypt_map[1] = 3; col[1] = 1; val_used[3] = true;
            decrypt_map[3] = 4; col[3] = 1; val_used[4] = true;
            decrypt_map[4] = 2; col[0] = 4; val_used[2] = true;
            decrypt_map[9] = 1; results[0] = 10; val_used[1] = true;
            break; // Assert: No continuation
    }

    columns[0] = col;

    verifyCol();

    cout << "Post Decrypt Map: ";
    for (int i = 0; i < 26; i++) {if (decrypt_map.at(i) != -1) {cout << "{" << i << " - " << decrypt_map.at(i) << "}, ";}}
    cout << endl;
    cout << "Post Val Used: ";
    for (int i = 0; i < 10; i++) {if (val_used.at(i)) {cout << i << " ";}}
    cout << endl;
    cout << "Post ColI: " << colI << endl;
    cout << "Post Carry: " << carry << endl;

    cout << "TASK END" << endl;
    return 0;
}