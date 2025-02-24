#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <random>

using namespace std;

// Configuration
int numEncryptions = 5;
int numEquationsPerEncryption = 1;
int numDigits = 4;

class CryptarithmSolver {
public:
    // Dataset Variables
    int numDigits;

    // Equation Data Structures
    vector<unordered_map<int, int>> columnsToFreq;
    vector<int> results;
    unordered_set<int> startingSymbols;
    int equationSolutions;

    // Current Solution Data
    vector<int> sol;
    vector<bool> valUsed;

    CryptarithmSolver(int digits): 
        numDigits(digits),
        columnsToFreq(digits),
        results(digits + 1, 0),
        equationSolutions(0),
        sol(26, -1),
        valUsed(10, false)
    {}

    void reset() {
        columnsToFreq = vector<unordered_map<int, int>>(numDigits);
        results = vector<int>(numDigits + 1, 0);
        startingSymbols.clear();
        sol = vector<int>(26, -1);
        valUsed = vector<bool>(10, false);
        equationSolutions = 0;
    }

    void calculateColumn(int col, int carry) {
        // Step: End condition
        if (col >= numDigits) {
            int resultSymbol = results[numDigits];
            bool validSol = false;

            if (resultSymbol != -1) {
                if (sol.at(resultSymbol) != -1) {validSol = (sol.at(resultSymbol) == carry);} // If resultSymbol has been set -> Check if carry matches
                else {validSol = !valUsed.at(carry);} // If resultSymbol has not been set -> Check if carry val hasn't been used
            } else {
                validSol = (carry == 0);
            }

            if (validSol) {equationSolutions++;}
            return;
        }

        unordered_map<int, int> &freq = columnsToFreq.at(col); // unordered_map<int, int>
        vector<int> unknown;
        int knownSum = 0;

        for (const auto &freqVal : freq) { // freqVal: symbol to freq
            if (sol.at(freqVal.first) != -1) {knownSum += sol.at(freqVal.first) * freqVal.second;}  // value of freqSymbol * freq
            else if (freqVal.first != -1) {unknown.push_back(freqVal.first);}
        }

        // Sort unknown by freq to improve pruning
        sort(unknown.begin(), unknown.end(), [&](int a, int b) {return freq.at(a) > freq.at(b);});

        assignIndex(col, unknown, knownSum, freq, carry, 0);
    }

    void assignIndex(int col, const vector<int> &unknown, int knownSum, unordered_map<int, int> &freq, int carry, int index) {
        if (index >= unknown.size()) {verifyColumn(col, unknown, knownSum, freq, carry); return;}
    
        int loopBound = min(9, results.at(col) + 20 - knownSum);
        int unknownSymbol = unknown.at(index);
        // If starting symbols does not include current symbol, 0 is valid. Vice versa
        for (int val = (startingSymbols.find(unknownSymbol) == startingSymbols.end()) ? 0 : 1; val <= loopBound; val++) {
            if (!valUsed.at(val)) {
                sol[unknownSymbol] = val;
                valUsed[val] = true;
    
                assignIndex(col, unknown, knownSum, freq, carry, index + 1);

                sol[unknownSymbol] = -1;
                valUsed[val] = false;
            }
        };
    }

    void verifyColumn(int col, const vector<int> &unknown, int knownSum, unordered_map<int, int> &freq, int carry) {
        for (int val : unknown) {knownSum += sol.at(val) * freq.at(val);}
        int remainder = (knownSum + carry) % 10;
        
        int resultSymbol = results.at(col);
        if (sol.at(resultSymbol) == -1) {
            sol[resultSymbol] = remainder;
            calculateColumn(col + 1, (knownSum + carry) / 10);
            sol[resultSymbol] = -1;
        } else if (sol.at(resultSymbol) == remainder) {
            calculateColumn(col + 1, (knownSum + carry) / 10);
        }
    }

    unordered_map<int, int> generateEncryptMap(std::uniform_int_distribution<> &distribSolution, std::mt19937 &gen) {
        unordered_map<int, int> encryptMap;
        vector<bool> encryptUsed(26);
        encryptMap[-1] = -1;

        for (int val = 0; val < 10; val++) {
            int key = distribSolution(gen);
            while (encryptUsed.at(key)) {key = distribSolution(gen);}

            encryptMap[val] = key;
            encryptUsed[key] = true;
        }

        return encryptMap;
    }

    void formatNumbers(unordered_map<int, int> &encryptMap, int A, int B, int C, int R, vector<int> &a, vector<int> &b, vector<int> &c) {
        for (int i = 0; i < numDigits; i++) {
            a[i] = A % 10; b[i] = B % 10; c[i] = C % 10; results[i] = R % 10;

            A /= 10; B /= 10; C /= 10; R /= 10;
        }
        results[numDigits] = R % 10; // Because result can be one more digit than numbers

        // Replace 0s in front w/ -1 encryptian mask and store starting values
        if (a[numDigits - 1] == 0) {
            startingSymbols.insert(encryptMap.at(a.at(numDigits - 2)));
            a[numDigits - 1] = -1;
        } else {
            startingSymbols.insert(encryptMap.at(a.at(numDigits - 1)));
        }
        if (b[numDigits - 1] == 0) {
            startingSymbols.insert(encryptMap.at(b.at(numDigits - 2)));
            b[numDigits - 1] = -1;
        } else {
            startingSymbols.insert(encryptMap.at(b.at(numDigits - 1)));
        }
        if (c[numDigits - 1] == 0) {
            startingSymbols.insert(encryptMap.at(c.at(numDigits - 2)));
            c[numDigits - 1] = -1;
        } else {
            startingSymbols.insert(encryptMap.at(c.at(numDigits - 1)));
        }
        if (results[numDigits] == 0) {
            startingSymbols.insert(encryptMap.at(results.at(numDigits - 2)));
            results[numDigits] = -1;
        } else {
            startingSymbols.insert(encryptMap.at(results.at(numDigits - 1)));
        }
    }

    void encrypt(unordered_map<int, int> &encryptMap, vector<int> &a, vector<int> &b, vector<int> &c) {
        for (int colI = 0; colI < numDigits; colI++) {
            unordered_map<int, int> &columnFreq = columnsToFreq[colI];
            int aEn = encryptMap.at(a.at(colI));
            int bEn = encryptMap.at(b.at(colI));
            int cEn = encryptMap.at(c.at(colI));

            columnFreq[aEn]++;
            columnFreq[bEn]++;
            columnFreq[cEn]++;

            results[colI] = encryptMap.at(results.at(colI));
        }
        results[numDigits] = encryptMap.at(results.at(numDigits));
    }
};

int main() {
    // Reserve data for recording equation difficulties
    vector<pair<string, int>> equationDifficulty;
    equationDifficulty.reserve(numEncryptions * numEquationsPerEncryption);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(pow(10, numDigits - 1), pow(10, numDigits) - 1);
    uniform_int_distribution<> distribSolution(0, 25);

    CryptarithmSolver solver(numDigits);

    // For number of encriptions
    for (int encryptionT = 0; encryptionT < numEncryptions; encryptionT++) {
        // Generate random encryption map
        unordered_map<int, int> encryptMap = solver.generateEncryptMap(distribSolution, gen);
    
        // For number of equations per encryption
        for (int equationT = 0; equationT < numEquationsPerEncryption; equationT++) {
            solver.reset();
    
            // Generate random equation
            int A = distrib(gen);
            int B = distrib(gen);
            int C = distrib(gen);
            int R = A + B + C;
    
            vector<int> a(numDigits), b(numDigits), c(numDigits);
    
            solver.formatNumbers(encryptMap, A, B, C, R, a, b, c);
            solver.encrypt(encryptMap, a, b, c);
    
            solver.calculateColumn(0, 0);
    
            string equationStr = to_string(A) + ", " + to_string(B) + ", " + to_string(C) + ", " + to_string(R);
            equationDifficulty.push_back(make_pair(equationStr, solver.equationSolutions));
        }
    }

    // Output generated dataset
    for (auto &eq : equationDifficulty) {cout << eq.first << " -> " << eq.second << " solutions" << "\n";}
    
    cout << "TASK END" << endl;
    return 0;
}
