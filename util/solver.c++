#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <array>
#include <random>
#include <algorithm>
using namespace std;

static long long toNumber(const string &s, const unordered_map<char,int> &assignment) {
    // Convert a letter-based string to a numeric value using the assignment
    // e.g. if s = "ABC", assignment['A']=9, assignment['B']=5, assignment['C']=7
    // then returns 957 as a long long
    long long val = 0;
    for (char ch : s) {
        val = val * 10 + assignment.at(ch);
    }
    return val;
}

int main(){
    string equa = "6221 + 5997 + 7479 = 19697";

    // Remove spaces and split by operators
    string A, B, C, R;
    size_t pos = 0;
    
    pos = equa.find("+");
    A = equa.substr(0, pos);
    A.erase(remove(A.begin(), A.end(), ' '), A.end());
    
    size_t lastPos = pos + 1;
    pos = equa.find("+", lastPos);
    B = equa.substr(lastPos, pos - lastPos);
    B.erase(remove(B.begin(), B.end(), ' '), B.end());
    
    lastPos = pos + 1;
    pos = equa.find("=", lastPos);
    C = equa.substr(lastPos, pos - lastPos);
    C.erase(remove(C.begin(), C.end(), ' '), C.end());
    
    R = equa.substr(pos + 1);
    R.erase(remove(R.begin(), R.end(), ' '), R.end());

    // Collect all distinct letters in A, B, C, R
    unordered_set<char> uniqueLetters;
    for (char ch : A) uniqueLetters.insert(ch);
    for (char ch : B) uniqueLetters.insert(ch);
    for (char ch : C) uniqueLetters.insert(ch);
    for (char ch : R) uniqueLetters.insert(ch);

    // Convert unordered_set to vector to have a stable ordering
    vector<char> letters(uniqueLetters.begin(), uniqueLetters.end());
    int n = (int)letters.size();

    // If more than 10 letters, no solution is possible:
    if (n > 10) {
        cout << "No unique solution (more than 10 symbols)!\n";
        return 0;
    }

    // We'll store all solutions (assignments) that satisfy the puzzle
    vector<unordered_map<char,int>> solutions;

    // A helper to check for leading zeros:
    // For each "word", if it has more than 1 character, then it cannot map to a number with leading zero
    auto isLeadingZero = [&](const string &s, const unordered_map<char,int> &assign) {
        if (s.size() > 1 && assign.at(s[0]) == 0) {
            return true; // invalid because leading digit is 0
        }
        return false;
    };

    // We will create a permutation of digits [0..9] for the letters
    // We do a depth-first search over the letters.
    vector<bool> used(10, false);  // used[d] = true if digit d is taken

    function<void(int, unordered_map<char,int>&)> backtrack = 
    [&](int idx, unordered_map<char,int> &assign) {
        // If we've assigned all letters
        if (idx == n) {
            // Check leading zero constraints
            if (isLeadingZero(A, assign)) return;
            if (isLeadingZero(B, assign)) return;
            if (isLeadingZero(C, assign)) return;
            if (isLeadingZero(R, assign)) return;

            // Compute numeric values
            long long valA = toNumber(A, assign);
            long long valB = toNumber(B, assign);
            long long valC = toNumber(C, assign);
            long long valR = toNumber(R, assign);

            if (valA + valB + valC == valR) {
                solutions.push_back(assign);

                
            }
            return;
        }

        // Current letter
        char letter = letters[idx];
        // Try assigning each digit 0..9 (that hasn't been used yet)
        for (int digit = 0; digit < 10; digit++) {
            if (!used[digit]) {
                used[digit] = true;
                assign[letter] = digit;

                backtrack(idx + 1, assign);

                // revert
                used[digit] = false;
                assign.erase(letter);
                
                // No pruning or optimization: we systematically try everything
            }
        }
    };

    // We'll keep an assignment map that we fill during backtracking
    unordered_map<char,int> assignment;
    backtrack(0, assignment);

    cout << "Equation: " << equa << "\n";
    cout << "Number of solutions: " << solutions.size() << "\n";
    for (int i = 0; i < solutions.size(); i++) {
        unordered_map<char,int> &assign = solutions[i];
        for (auto& [key, symbol] : assign) {
            cout << key << "-" << symbol << ", ";
        }
        cout << endl;
    }

    return 0;
}
