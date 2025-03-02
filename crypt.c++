// TODO: Add symbols cannot be leading zero thing

#include <iostream>
#include <fstream>
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
    array<int, 4> leadingSymbols;

    array<int, 26> decrypt_map; // Symbol to Value - If (val == -1) -> Symbol not assigned
    array<bool, 10> val_used;

    int colI;
    int carry;

    int current_solutions;

    void solveColumn() {
        if (colI == num_digits) {
            if (verify_solution()) {current_solutions++;}
            if (display_code >= 0) {
                cout << "Decrypt Map: ";
                for (int s = 0; s < 26; s++) {if (decrypt_map.at(s) != -1) {cout << s << "-" << decrypt_map.at(s) << ", ";}}
                cout << endl;
            }
            return;
        }

        if (display_code >= 1) {cout << "Solve Column: " << colI << endl;}
        if (display_code >= 2) {
            cout << "SC - Columns" << endl;
            for (int s = 0; s < 26; s++) {if (columns.at(colI).at(s) != 0) {cout << s << "-" << columns.at(colI).at(s) << ", ";}}
            cout << endl;
        }

        array<int, 26> &col = columns.at(colI); // Symbol to Frequency
        int known_sum = carry;  // Remember this includes carry
        array<int, 10> unknown; // Symbols to Value
        unknown.fill(-1);

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

        // Recursive function that explores tree
        assignUnknown(known_sum, unknown);
    }

    void assignUnknown(int &known_sum, array<int, 10> &unknown) {
        if (display_code >= 2) {cout << "Assign Unknown - ";}
        if (display_code >= 3) {
            cout << "Unknown: ";
            for (int i = 0; i < 10 && unknown.at(i) != -1; i++) {cout << unknown.at(i) << ", ";}
        }
        if (display_code >= 2) {cout << endl;}

        array<int, 26> &col = columns.at(colI); // Symbol to Frequency

        int unknownI = 0;
        while (unknownI >= 0) {
            if (display_code >= 3) {
                cout << "UnknownI: " << unknownI << endl;
                cout << "AU - Decrypt Map: ";
                for (int s = 0; s < 26; s++) {if (decrypt_map.at(s) != -1) {cout << s << "-" << decrypt_map.at(s) << ", ";}}
                cout << endl;
            }

            int unknownS = unknown[unknownI];

            if (unknownS == -1) {
                stepCol();
                unknownI--;
                continue;
            }

            int next_val = decrypt_map.at(unknownS);
            if (next_val != -1) {
                decrypt_map[unknownS] = -1;
                val_used[next_val] = false;
            }

            next_val++;
            while (next_val < 10 && val_used[next_val]) {next_val++;}

            if (next_val == 10) {
                unknownI--;
                continue;
            } else {
                decrypt_map[unknownS] = next_val;
                val_used[next_val] = true;
                unknownI++;
                continue;
            }
        }
    }

    void stepCol() {
        if (display_code >= 2) {cout << "Step Col" << colI << endl;}
        if (display_code >= 3) {
            cout << "SC - Decrypt Map: ";
            for (int s = 0; s < 26; s++) {if (decrypt_map.at(s) != -1) {cout << s << "-" << decrypt_map.at(s) << ", ";}}
            cout << endl;
        }

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
        if (display_code >= 2) {cout << "Verify Solution" << endl;}

        // Assert: No leading zeros
        for (int symbol : leadingSymbols) {
            if (symbol != -1 && decrypt_map.at(symbol) == 0) {
                if (display_code >= 2) {cerr << "ERROR: Leading symbol " << symbol << " assigned 0" << endl;}
                return false;
            }
        }

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
        leadingSymbols.fill(-1);
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
        leadingSymbols.fill(-1);
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
        if (display_code >= 1) {cout << "Process Equation" << endl;}
        
        for (int i = num_digits - 1; i >= 0; i--) {
            if (a[i] >= 0 && leadingSymbols[0] == -1) {leadingSymbols[0] = a[i];}
            if (b[i] >= 0 && leadingSymbols[1] == -1) {leadingSymbols[1] = b[i];}
            if (c[i] >= 0 && leadingSymbols[2] == -1) {leadingSymbols[2] = c[i];}
            if (r[i] >= 0 && leadingSymbols[3] == -1) {leadingSymbols[3] = r[i];}
        }

        for (int i = 0; i < num_digits; i++) {
            columns[i].fill(0);
            if (a.at(i) != -1) {columns[i][a[i]]++;}
            if (b.at(i) != -1) {columns[i][b[i]]++;}
            if (c.at(i) != -1) {columns[i][c[i]]++;}

            if (display_code >= 2) {
                cout << "Column: " << i << " - ";
                for (int s  = 0; s < 26; s++) {if (columns.at(i).at(s) != 0) {cout << s << "-" << columns.at(i).at(s) << ", ";}}
                cout << "Result: " << r.at(i) << endl;
            }
        }

        results = r;

        // Leading zeros are set to the symbol -1
    }
};