#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <random>
using namespace std;

string filename = "cryptarithm.out";
ofstream fout(filename);

void calculateColumn(int col, int carry);
void assignIndex(int col, vector<int> unknown, int knownSum, unordered_map<int, int> freq, int carry, int index);
void verifyColumn(int col, vector<int> unknown, int knownSum, unordered_map<int, int> freq, int carry);
unordered_map<int, int> generateEncryptMap(std::uniform_int_distribution<> distribSolution, std::mt19937 gen);
void resetGlobalDataStructures();
unordered_set<int> formatNumbers(int A, int B, int C, int R, vector<int> &a, vector<int> &b, vector<int> &c);
void calculateColumnFreq(unordered_map<int, int> &columnFreq, int aEn, int bEn, int cEn);

bool DEBUG = true;     // General logs

bool DEBUGC = false && DEBUG;   // Logs the maximum column reached in solution space exploration
bool DEBUGI = true && DEBUG;    // Logs the number of paths explored during solution space exploration
int maxColumn = 0; int iterations = 0;

bool DEBUGS = false && DEBUG;   // Logs the solution attempts that reach the final verification (Doesn't mean they pass)
bool DEBUGLIM = false && DEBUG; // Limits solution space exloration using maxColumnDEBUG & maxIndexDEBUG
int maxColumnDEBUG = 1; int maxIndexDEBUG = 3;


// Dataset Variables
int numDigits = 4;
int numEquationsPerEncrption = 100;

vector<pair<string, int>> equationDifficulty;

// Equation Data Structure
vector<unordered_map<int, int>> columnsToFreq(numDigits);
vector<int> results(numDigits + 1);
unordered_set<int> startingSymbols;
int equationSolutions;

// Current Solution Data
unordered_map<int, int> sol(26);
vector<bool> valUsed(26, false);

void calculateColumn(int col, int carry) {
    if (DEBUGLIM && col >= maxColumnDEBUG) {return;} // TODO: REMOVE

    // Step: End condition
    if (col >= numDigits) {
        int resultSymbol = results[numDigits];
        bool validSol = false;

        if (sol.count(resultSymbol) == 1) {validSol = (sol[resultSymbol] == carry);} // If resultSymbol has been set -> Check if carry matches
        else {validSol = !valUsed[carry];} // If resultSymbol has not been set -> Check if carry val hasn't been used

        if (validSol) {
            equationSolutions++;

            if (DEBUG || DEBUGS) { // Logs the solution
                std::cout << "Solution Found: ";
                for (auto &symbolval : sol) {std::cout << symbolval.first << "-" << symbolval.second << ", ";}
                std::cout << endl;
            }
        } else if (DEBUGS) { // Logs the failed solution
            std::cout << "Solution !Found: ";
            for (auto &symbolval : sol) {std::cout << symbolval.first << "-" << symbolval.second << ", ";}
            std::cout << endl;
        }
        return;
    }

    unordered_map<int, int> freq = columnsToFreq[col]; // unordered_map<int, int>
    vector<int> unknown;
    int knownSum = 0;

    for (auto &freqVal : freq) { // freqVal: symbol to freq
        auto it = sol.find(freqVal.first);
        if (it != sol.end()) {knownSum += it->second * freqVal.second;}  // value of freqSymbol * freq
        else if (freqVal.first != -1) {unknown.push_back(freqVal.first);}
    }

    assignIndex(col, unknown, knownSum, freq, carry, 0);
}

void assignIndex(int col, vector<int> unknown, int knownSum, unordered_map<int, int> freq, int carry, int index) {
    if (index >= unknown.size()) {
        verifyColumn(col, unknown, knownSum, freq, carry);
        return;
    }

    if (DEBUGLIM && index >= maxIndexDEBUG) {return;} // TODO: REMOVE
    if (DEBUGI) {iterations++;}

    bool zeroValid = (startingSymbols.find(unknown[index]) == startingSymbols.end()); // If starting symbols does not include current symbol, 0 is valid. Vice versa
    for (int val = zeroValid ? 0 : 1; val <= min(9, results[col] + 20 - knownSum); val++) {
        if (!valUsed[val]) {
            sol[unknown[index]] = val;
            valUsed[val] = true;

            // REMOVE
            fout << "Index: " << index << " Symbol: " << unknown[index] << " --- ";
            for (auto &pair : sol) {fout << pair.first << "-" << pair.second << ", ";}
            fout << endl;
            // REMOVE

            assignIndex(col, unknown, knownSum, freq, carry, index + 1);

            sol.erase(unknown[index]);
            valUsed[val] = false;
        }
    }
}

void verifyColumn(int col, vector<int> unknown, int knownSum, unordered_map<int, int> freq, int carry) {
    int resultSymbol = results[col];

    for (int val : unknown) {knownSum += sol[val] * freq[val];}

    int remainder = (knownSum + carry) % 10;

    auto it = sol.find(resultSymbol);
    if (it == sol.end()) {
        if (DEBUGC) {maxColumn = max(col + 1, maxColumn);}

        sol[resultSymbol] = remainder;

        calculateColumn(col + 1, (knownSum + carry) / 10);
        fout << "Verify Solution: ----- " << "Final Sum: " << knownSum << " Remainder: " << remainder << " Solution Symbol Value: " << sol[resultSymbol] << endl;

        sol.erase(resultSymbol);
    } else if (it->second == remainder) {
        if (DEBUGC) {maxColumn = max(col + 1, maxColumn);}

        calculateColumn(col + 1, (knownSum + carry) / 10);
        fout << "Verify Solution: ----- " << "Final Sum: " << knownSum << " Remainder: " << remainder << " Solution Symbol Value: " << sol[resultSymbol] << endl;
    } else {
        fout << "Verify Solution: ----- " << "Final Sum: " << knownSum << " Remainder: " << remainder << " Solution Symbol Value: null" << endl;
    }
}

// TEST MAIN
void testMain() {
    equationDifficulty.reserve(numEquationsPerEncrption);
    columnsToFreq.reserve(4);

    int A = 903;
    int B = 100;
    int C = 250;
    int R = A + B + C;

    vector<int> a(numDigits);
    vector<int> b(numDigits);
    vector<int> c(numDigits);

    unordered_set<int> startingValues = formatNumbers(A, B, C, R, a, b, c);

    for (int colI = 0; colI < numDigits; colI++) {
        unordered_map<int, int> &columnFreq = columnsToFreq[colI];

        int aEn = a[colI];
        int bEn = b[colI];
        int cEn = c[colI];

        // // Format equation
        calculateColumnFreq(columnFreq, aEn, bEn, cEn);

        results[colI] = results[colI];

        a[colI] = aEn;
        b[colI] = bEn;
        c[colI] = cEn;
    }
    results[numDigits] = results[numDigits];

    fout << "Encrypted Equation (Reversed)\n";
    for (int i = 0; i < numDigits; i++) {fout << a[i] << " ";} fout << "\n";
    for (int i = 0; i < numDigits; i++) {fout << b[i] << " ";} fout << "\n";
    for (int i = 0; i < numDigits; i++) {fout << c[i] << " ";}; fout << "\n";
    for (int i = 0; i < numDigits + 1; i++) {fout << results[i] << " ";}; fout << "\n\n";

    calculateColumn(0, 0);

    std::cout << "Equation Solutions: " << equationSolutions << endl;
    fout << "Equation Solutions: " << equationSolutions << endl;
}

int main() {
    if (false) {testMain(); return 0;}

    equationDifficulty.reserve(numEquationsPerEncrption);
    columnsToFreq.reserve(4);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distrib(pow(10, numDigits - 1), pow(10, numDigits) - 1);
    std::uniform_int_distribution<> distribSolution(1, 26);

    // Step: Generate random encryption map
    unordered_map<int, int> encryptMap = generateEncryptMap(distribSolution, gen);

    // Step: For number of equations per encryption map
    for (int t = 0; t < numEquationsPerEncrption; t++) {
        std::cout << "\nEquation " << (t - numEquationsPerEncrption) << endl;

        // Step: Reset all global data structures
        resetGlobalDataStructures();

        // Step: Generate random equation
        int A = distrib(gen);
        int B = distrib(gen);
        int C = distrib(gen);
        int R = A + B + C;

        string equation = to_string(A) + ", " + to_string(B) + ", " + to_string(C) + ", " + to_string(R);
        std::cout << equation << "\n"
                  << endl;

        // Step: Process equation & save starting values
        vector<int> a(numDigits);
        vector<int> b(numDigits);
        vector<int> c(numDigits);

        unordered_set<int> startingValues = formatNumbers(A, B, C, R, a, b, c);

        // Step: Encrypt digits and starting values & format equation
        for (int colI = 0; colI < numDigits; colI++)
        {
            unordered_map<int, int> &columnFreq = columnsToFreq[colI];

            // Encrypt digits
            int aEn = encryptMap.at(a[colI]);
            int bEn = encryptMap.at(b[colI]);
            int cEn = encryptMap.at(c[colI]);
            results[colI] = encryptMap[results[colI]];

            // Format equation
            calculateColumnFreq(columnFreq, aEn, bEn, cEn);

            if (DEBUG) { // DEBUG: For encryptian logging
                a[colI] = aEn;
                b[colI] = bEn;
                c[colI] = cEn;
            }
        }
        results[numDigits] = encryptMap[results[numDigits]];

        for (auto &value : startingValues) {
            startingSymbols.insert(encryptMap[value]);
        }

        if (DEBUG) {
            std::cout << "Encrypted Equation (Reversed)\n";
            for (int i = 0; i < numDigits; i++) {std::cout << a[i] << " ";} std::cout << "\n";
            for (int i = 0; i < numDigits; i++) {std::cout << b[i] << " ";} std::cout << "\n";
            for (int i = 0; i < numDigits; i++) {std::cout << c[i] << " ";} std::cout << "\n";
            for (int i = 0; i < numDigits + 1; i++) {std::cout << results[i] << " ";} std::cout << "\n\n";

            std::cout << "Starting Symbosl\n";
            for (auto &startingSymbol : startingSymbols) {std::cout << startingSymbol << ", ";}
            std::cout << "\n" << endl;
        }

        // Step: Solve for solutions & save solutions data
        if (DEBUG) {std::cout << "Solving Equation" << endl;}

        calculateColumn(0, 0); // Recursive function to test all decryptian w/ a few inbetween checks for optimization

        // Step: Record data
        equationDifficulty.push_back(make_pair(equation, equationSolutions));

        if (DEBUGC) {std::cout << "Max Column: " << maxColumn << endl;}
        if (DEBUGI) {std::cout << "Paths Explored: " << iterations << endl;}
    }

    std::cout << "\nEquations Difficulty\n";
    for (auto &equations : equationDifficulty) {std::cout << equations.first << " " << to_string(equations.second) << "\n";}
    std::cout << "TASK END" << endl;
}

unordered_map<int, int> generateEncryptMap(std::uniform_int_distribution<> distribSolution, std::mt19937 gen) {
    if (DEBUG) {std::cout << "Generating encryption map...\n";}

    unordered_map<int, int> encryptMap; // value to key
    vector<bool> encryptUsed(26);       // keys used
    encryptMap[-1] = -1;

    for (int val = 0; val < 10; val++) {
        // Generate valid key
        int key = distribSolution(gen);
        while (encryptUsed[key - 1]) {
            key = distribSolution(gen);
        }

        // Store val to key encryption & update valid keys
        encryptMap[val] = key;
        encryptUsed[key - 1] = true;

        if (DEBUG) {std::cout << val << " " << key << "\n";}
    }
    if (DEBUG) {std::cout << endl;}

    return encryptMap;
}

void resetGlobalDataStructures() {
    columnsToFreq = vector<unordered_map<int, int>>(numDigits);
    results = vector<int>(numDigits + 1);
    startingSymbols.clear();
    sol = unordered_map<int, int>(26);
    valUsed = vector<bool>(26, false);
    equationSolutions = 0;
}

unordered_set<int> formatNumbers(int A, int B, int C, int R, vector<int> &a, vector<int> &b, vector<int> &c) {
    // Store numbers as vector of digits
    for (int i = 0; i < numDigits; i++) {
        a[i] = A % 10;
        b[i] = B % 10;
        c[i] = C % 10;
        results[i] = R % 10;

        A /= 10;
        B /= 10;
        C /= 10;
        R /= 10;
    }
    results[numDigits] = R % 10; // Because result can be one more digit than numbers

    // Replace 0s in front w/ -1 encryptian mask and store starting symbols
    unordered_set<int> startingValues;
    if (a[numDigits - 1] == 0) {
        startingValues.insert(a[numDigits - 2]);
        a[numDigits - 1] = -1;
    } else {
        startingValues.insert(a[numDigits - 1]);
    }
    if (b[numDigits - 1] == 0) {
        startingValues.insert(b[numDigits - 2]);
        b[numDigits - 1] = -1;
    } else {
        startingValues.insert(b[numDigits - 1]);
    }
    if (c[numDigits - 1] == 0) {
        startingValues.insert(c[numDigits - 2]);
        c[numDigits - 1] = -1;
    } else {
        startingValues.insert(c[numDigits - 1]);
    }
    if (results[numDigits] == 0) {
        startingValues.insert(results[numDigits - 1]);
        results[numDigits] = -1;
    } else {
        startingValues.insert(b[numDigits - 1]);
    }

    return startingValues;
}

void calculateColumnFreq(unordered_map<int, int> &columnFreq, int aEn, int bEn, int cEn) {
    columnFreq[aEn] = 0;
    columnFreq[bEn] = 0;
    columnFreq[cEn] = 0;

    columnFreq[aEn]++;
    columnFreq[bEn]++;
    columnFreq[cEn]++;
}