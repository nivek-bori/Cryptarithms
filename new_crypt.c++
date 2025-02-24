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
    int num_digits;


    vector<array<int, 26>> columns; // Columns to Frequence - Index: Symbol, Value: Frequency
    
    array<int, 26> decrypt_map; // Symbol to Value - If (val == -1) -> Symbol not assigned
    array<bool, 10> val_used;

    CryptarithmSolver(int digits):
        num_digits(digits),
        columns(digits)
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
        
    }
};