// Verify Column Test
// Unit Test: TODO

#include <iostream>
#include <vector>
#include <array>
using namespace std;

vector<array<int, 26>> columns; // Index: Column, Value: <Index: Symbol, Value: Frequency>
vector<int> results; // Index: Column, Value: Symbol

array<int, 26> decrypt_map; // Symbol to Value - If (val == -1) -> Symbol not assigned
array<bool, 10> val_used;

int colI;
int carry;

bool testing_flag;

void verifyCol() {
    array<int, 26> &col = columns.at(colI); // Symbol to Frequency
    int resultS = results.at(colI);

    // Assert: All symbols in column are assigned
    for (int assertS = 0; assertS < 26; assertS++) {
        if (col.at(assertS) != -1 && decrypt_map.at(assertS) == -1) {
            cerr << "ERROR: Symbol in equation not assigned" << endl;
            return;
        }
    }

    int sum = carry;
    for (int s = 0; s < 26; s++) {
        if (col.at(s) != -1) {
            sum += decrypt_map.at(s) * col.at(s);
        }
    }

    if (decrypt_map.at(resultS) != -1) {
        if (decrypt_map.at(resultS) == carry) {
            testing_flag = true; // TODO: REMOVE

            colI++;
            int old_carry = carry;
            carry = sum / 10;
            
            // solveColumn();
            
            colI--;
            carry = old_carry;
        } else {
            testing_flag = false; // TODO: REMOVE
        }
    } else if (!val_used.at(carry)) {
        testing_flag = true; // TODO: REMOVE

        decrypt_map[resultS] = carry;
        val_used[carry] = true;
        
        colI++;
        int old_carry = carry;
        carry = sum / 10;
        
        // solveColumn();
        
        decrypt_map[resultS] = -1;
        val_used[carry] = false;

        colI--;
        carry = old_carry;
    } else {
        testing_flag = false; // TODO: REMOVE
    }
}

vector<array<int, 26>> columns; // Index: Column, Value: <Index: Symbol, Value: Frequency>
vector<int> results;            // Index: Column, Value: Symbol

array<int, 26> decrypt_map; // Symbol to Value - If (val == -1) -> Symbol not assigned
array<bool, 10> val_used;

int colI;
int carry;

bool testing_flag;

int main() {
    array<int, 26> col;
    col.fill(-1);

    decrypt_map.fill(-1);
    val_used.fill(false);

    colI = 0;
    carry = 0;

    testing_flag = false;

    int unit_test_num = 0;
    switch (unit_test_num) {
        case 0: // Not all symbols assigned
            decrypt_map[1] = 2; col[1] = 1;
            col[3] = 2;
            results[0] = 4;
            break;
        case 1: // Result symbol assigned correctly
            decrypt_map[1] = 2; col[1] = 1;
            decrypt_map[3] = 3; col[3] = 1;
            results[0] = 5;
            break;
        case 2: // Result symbol assigned incorrectly
            decrypt_map[1] = 2; col[1] = 1;
            decrypt_map[3] = 3; col[3] = 1;
            results[0] = 4;
            break;
        case 3: // Result symbol not assigned
            decrypt_map[1] = 2; col[1] = 1;
            decrypt_map[3] = 3; col[3] = 1;
            decrypt_map[6] = 3; col[6] = 1;
            break;
    }

    // TODO: Add more unit test cases
    // TODO: Store and log everything that changed - testing flag, decrypt_map, val_used, colI, carry
    // TODO: Log everything that changed & was reverted - testing flag, decrypt_map, val_used, colI, carry
}