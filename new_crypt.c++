#include <iostream>
#include <vector>
#include <array>

using namespace std;

class CryptarithmSolver {
private:
    int num_digits;

    // Reversed - Index 0 means smallest digit
    vector<array<int, 26>> columns; // Index: Column, Value: <Index: Symbol, Value: Frequency>
    vector<int> results; // Index: Column, Value: Symbol

    array<int, 26> decrypt_map; // Symbol to Value - If (val == -1) -> Symbol not assigned
    array<bool, 10> val_used;

    int colI;
    int carry;

    void solveColumn() {
        if (colI == num_digits) {
            // Assert: All columns solved
            
            
        }

        auto &col = columns[colI]; // Symbol to Frequency

        int known_sum = carry; // Remember this includes carry
        array<bool, 26> unknown; // Symbols to Value
        processCol(col, known_sum, unknown);
        
        // Recursive function that explores tree
        assignUnknown(0, known_sum, unknown);
    }

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

    void assignUnknown(int checkS, int &known_sum, array<bool, 26> &unknown) {
        for (; checkS < 26; checkS++) {
            if (unknown.at(checkS)) {
                for (int val = 0; val < 10; val++) {
                    if (!val_used.at(val)) {
                        decrypt_map[checkS] = val;
                        val_used[val] = true;
                        known_sum += val;

                        assignUnknown(checkS + 1, known_sum, unknown);

                        decrypt_map[checkS] = -1;
                        val_used[val] = false;
                        known_sum -= val;
                    }
                }

                return;
            }
        }

        // Given currnet decrypt_map, verify column
        stepCol();
    }

    void stepCol() {
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
        int remainder = sum % 10;

        if (decrypt_map.at(resultS) != -1) {
            if (decrypt_map.at(resultS) == remainder) {
                colI++;
                int old_carry = carry;
                carry = sum / 10;
                
                solveColumn();
                
                colI--;
                carry = old_carry;
            }
        } else if (!val_used.at(remainder)) {
            decrypt_map[resultS] = remainder;
            val_used[remainder] = true;
            
            colI++;
            int old_carry = carry;
            carry = sum / 10;
            
            solveColumn();

            decrypt_map[resultS] = -1;
            val_used[remainder] = false;

            colI--;
            carry = old_carry;
        }
    }

    bool verify_solution() {
        // Assert: All columns correctly solved
        int carryLoc = 0;
            for (int cI = 0; cI < num_digits; cI++) {
                array<int, 26> &col = columns.at(cI); // Symbol to Frequency
                int resultS = results.at(cI);
                
                // Assert: All symbols in column are assigned
                for (int s = 0; s < 26; s++) {
                    if (col.at(s) != -1 && decrypt_map.at(s) == -1) {
                        cerr << "ERROR: Symbol in equation not assigned" << endl;
                        return;
                    }
                }
                if (decrypt_map.at(resultS) == -1) {
                    cerr << "ERROR: Result symbol not assigned" << endl;
                    return;
                }

                int colSum = carryLoc;
                for (int s = 0; s < 26; s++) {
                    if (col.at(s) != -1) {
                        colSum += decrypt_map.at(s) * col.at(s);
                    }
                }

                int remainder = colSum % 10;
                if (decrypt_map.at(resultS) != remainder) {
                    cerr << "ERROR: Result value incorrect" << endl;
                    return;
                }

                carryLoc = colSum / 10;
            }

            if (carryLoc != 0) {
                cerr << "ERROR: Final carry not 0" << endl;
                return;
            }
    }

public:
    CryptarithmSolver(int digits):
        num_digits(digits + 1),
        columns(digits + 1)
    {
        decrypt_map.fill(-1);
        val_used.fill(false);
        colI = 0;
        carry = 0;
    }

    void reset() {
        columns = vector<array<int, 26>>(num_digits);
        results = vector<int>(num_digits);
        decrypt_map.fill(-1);
        val_used.fill(false);
        colI = 0;
        carry = 0;
    }

    int solve(vector<int> a, vector<int> b, vector<int> c, vector<int> r) {
        if (!verify_equation) {
            cerr << "Invalid equation" << endl;
            return -1;
        }

        process_equation(a, b, c, r);

        // TODO: Rest of logic
    }

    bool verify_equation(vector<int> a, vector<int> b, vector<int> c, vector<int> r) {
        // Numbers are valid size
        if (a.size() != num_digits) {return false;}
        if (b.size() != num_digits) {return false;}
        if (c.size() != num_digits) {return false;}
        if (r.size() != num_digits) {return false;}

        // Symbols are valid
        for (int i = 0; i < num_digits; i++) {
            if (a[i] < 0 || a[i] >= 26) {return false;}
            if (b[i] < 0 || b[i] >= 26) {return false;}
            if (c[i] < 0 || c[i] >= 26) {return false;}
            if (r[i] < 0 || r[i] >= 26) {return false;}
        }

        return true;
    }

    void process_equation(vector<int> a, vector<int> b, vector<int> c, vector<int> r) {
        for (int i = 0; i < num_digits; i++) {
            columns[i][a[i]]++;
            columns[i][b[i]]++;
            columns[i][c[i]]++;

            results[i] = r[i];
        }

        // Leading zeros are set to the symbol -1
    }
};