// TODO: Fix assignUnknown where s == 25 isn't unknown so it goes to i == 26 and then it goes back to s == 25

#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>

using namespace std;

class CryptarithmSolver {
private:
    int num_digits;
    int display_code;

    // Reversed - Index 0 means smallest digit
    vector<array<int, 26> > columns; // Index: Column, Value: <Index: Symbol, Value: Frequency>
    vector<int> results; // Index: Column, Value: Symbol

    array<int, 26> decrypt_map; // Symbol to Value - If (val == -1) -> Symbol not assigned
    array<bool, 10> val_used;

    int colI;
    int carry;

    int current_solutions;

    void solveColumn() {
        if (display_code >= 1) {cout << "Solve Column" << endl;}
        if (colI == num_digits) {
            // Assert: All columns solved
            if (verify_solution()) {
                current_solutions++;
            }

            return;
        }

        auto &col = columns.at(colI); // Symbol to Frequency

        int known_sum = carry; // Remember this includes carry
        array<int, 10> unknown; // Symbols to Value
        processCol(col, known_sum, unknown);
        
        // Recursive function that explores tree
        assignUnknown(known_sum, unknown);
    }

    // Return is passed through referenced - known_sum & unknown
    void processCol(array<int, 26> &col, int &known_sum, array<int, 10> &unknown) {
        if (display_code >= 1) {cout << "Process Column" << endl;}

        known_sum = carry; // Referenced return
        unknown.fill(-1); // // Referenced return

        int unknownI = 0;
        for (int s = 0; s < 26; s++) {
            if (col.at(s) != 0) {
                if (decrypt_map.at(s) != -1) {
                    known_sum += decrypt_map.at(s) * col.at(s); // Value of (s) * Frequency of (s)
                } else {
                    unknown[unknownI++] = s;
                }
            }
        }
    }

    void assignUnknown(int &known_sum, array<bool, 26> &unknown) {
        if (display_code >= 1) {cout << "Assign Unknown" << endl;}

        array<int, 26> &col = columns.at(colI); // Symbol to Frequency

        int unknownI = 0;
        while (unknownI >= 0) {
            int next_val = decrypt_map.at(unknown[unknownI]);
            while (next_val < 10 && val_used[next_val]){next_val++;}

            if (next_val == 10) {
                decrypt_map[unknown[unknownI]] = -1;
                val_used[next_val] = false;
                unknownI--;
                continue;
            } else {
                decrypt_map[unknown[unknownI]] = next_val;
                val_used[next_val] = true;
                unknownI++;
            }
        }
    }

    void stepCol() {
        if (display_code >= 1) {cout << "Step Col" << endl;}
        array<int, 26> &col = columns.at(colI); // Symbol to Frequency
        int resultS = results.at(colI);

        // Assert: All symbols in column are assigned
        for (int assertS = 0; assertS < 26; assertS++) {
            if (col.at(assertS) != 0 && decrypt_map.at(assertS) == -1) {
                cerr << "ERROR: Symbol in equation not assigned" << endl;
                return;
            }
        }

        int sum = carry;
        for (int s = 0; s < 26; s++) {
            if (col.at(s) != 0) {
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
        if (display_code >= 1) {cout << "Verify Solution" << endl;}

        // Assert: All columns correctly solved
        int carryLoc = 0;
        for (int cI = 0; cI < num_digits; cI++) {
            array<int, 26> &col = columns.at(cI); // Symbol to Frequency
            int resultS = results.at(cI);
            
            // Assert: All symbols in column are assigned
            for (int s = 0; s < 26; s++) {
                if (col.at(s) != 0 && decrypt_map.at(s) == -1) {
                    cerr << "ERROR: Symbol in equation not assigned" << endl;
                    return false;
                }
            }
            if (decrypt_map.at(resultS) == -1) {
                cerr << "ERROR: Result symbol not assigned" << endl;
                return false;
            }

            int colSum = carryLoc;
            for (int s = 0; s < 26; s++) {
                if (col.at(s) != 0) {
                    colSum += decrypt_map.at(s) * col.at(s);
                }
            }

            int remainder = colSum % 10;
            if (decrypt_map.at(resultS) != remainder) {
                cerr << "ERROR: Result value incorrect" << endl;
                return false;
            }

            carryLoc = colSum / 10;
        }

        if (carryLoc != 0) {
            cerr << "ERROR: Final carry not 0" << endl;
            return false;
        }

        return true;
    }

public:
    CryptarithmSolver(int digits, int display_code):
        num_digits(digits + 1),
        display_code(display_code),
        columns(digits + 1)
    {
        decrypt_map.fill(-1);
        val_used.fill(false);
        colI = 0;
        carry = 0;
        current_solutions = 0;
    }

    void reset() {
        columns = vector<array<int, 26> >(num_digits);
        for (int i = 0; i < num_digits; i++) {columns[i].fill(0);}
        results = vector<int>(num_digits);
        decrypt_map.fill(-1);
        val_used.fill(false);
        colI = 0;
        carry = 0;
        current_solutions = 0;
    }

    int solve(vector<int> a, vector<int> b, vector<int> c, vector<int> r) {
        reset();

        int verify_equation_flag = verify_equation(a, b, c, r);
        if (verify_equation_flag != 0) {
            cerr << "Invalid equation" << endl;
            return verify_equation_flag;
        }
        process_equation(a, b, c, r);

        solveColumn(); // Start of solve

        return current_solutions;
    }

    int verify_equation(vector<int> a, vector<int> b, vector<int> c, vector<int> r) {
        // Numbers are valid size
        if (a.size() != num_digits) {return -1;}
        if (b.size() != num_digits) {return -1;}
        if (c.size() != num_digits) {return -1;}
        if (r.size() != num_digits) {return -1;}

        // Symbols are valid
        for (int i = 0; i < num_digits; i++) {
            if (a.at(i) < -1 || a.at(i) >= 26) {return -2;}
            if (b.at(i) < -1 || b.at(i) >= 26) {return -2;}
            if (c.at(i) < -1 || c.at(i) >= 26) {return -2;}
            if (r.at(i) < -1 || r.at(i) >= 26) {return -2;}
        }

        return 0;
    }

    void process_equation(vector<int> &a, vector<int> &b, vector<int> &c, vector<int> &r) { // Encrypted
        for (int i = 0; i < num_digits; i++) {
            columns[i].fill(0);
            if (a.at(i) != -1) {columns[i][a[i]]++;}
            if (b.at(i) != -1) {columns[i][b[i]]++;}
            if (c.at(i) != -1) {columns[i][c[i]]++;}
        }

        results = r;

        // Leading zeros are set to the symbol -1
    }
};