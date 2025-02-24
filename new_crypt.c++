#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <random>

using namespace std;

class CryptarithmSolver {
public:
    int num_digits; // Data structures are size num_digits + 1 to account for result possibly being 1 more digit

    // Columns to Frequence - Index: Symbol, Value: Frequency
    // Reversed - Index 0 means smallest digit
    vector<array<int, 26>> columns; 
    
    array<int, 26> decrypt_map; // Symbol to Value - If (val == -1) -> Symbol not assigned
    array<bool, 10> val_used;

    CryptarithmSolver(int digits):
        num_digits(digits),
        columns(digits + 1)
    {
        decrypt_map.fill(-1);
        val_used.fill(false);
    }

    void reset() {
        columns = vector<array<int, 26>>(num_digits);
        decrypt_map.fill(-1);
        val_used.fill(false);
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
        if (a.size() != num_digits + 1) {return false;}
        if (b.size() != num_digits + 1) {return false;}
        if (c.size() != num_digits + 1) {return false;}
        if (r.size() != num_digits + 1) {return false;}

        // Symbols are valid
        for (int i = 0; i < num_digits + 1; i++) {
            if (a[i] < 0 || a[i] >= 26) {return false;}
            if (b[i] < 0 || b[i] >= 26) {return false;}
            if (c[i] < 0 || c[i] >= 26) {return false;}
            if (r[i] < 0 || r[i] >= 26) {return false;}
        }

        return true;
    }

    void process_equation(vector<int> a, vector<int> b, vector<int> c, vector<int> r) {
        for (int i = 0; i < num_digits + 1; i++) {
            columns[i][a[i]]++;
            columns[i][b[i]]++;
            columns[i][c[i]]++;
            columns[i][r[i]]++;
        }
    }
};