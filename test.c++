#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

class CryptarithmSolver
{
private:
    // Each column: frequency of symbols in that column
    // Key: symbol (char), Value: how many times it appears
    std::vector<std::unordered_map<char, int>> columns;

    // The result symbol for each column
    std::vector<char> results;

    // Decrypt map: symbol -> digit assigned
    // If -1, symbol is not yet assigned
    std::unordered_map<char, int> decryptMap;

    // Which digits [0..9] have already been used
    bool valUsed[10] = {false};

    // Keep track of total solutions found
    int currentSolutionsCount = 0;

    // Recursive function to solve columns
    void solveColumn(int colI, int carry)
    {
        // Base case: if we've assigned all columns,
        // check whether the final carry is 0
        if (colI == (int)columns.size()) {
            if (carry == 0) {
                // Found a valid complete assignment
                currentSolutionsCount++;
            }
            return;
        }

        // Sum the digits for this column using currently assigned values
        // or try new assignments if needed
        int columnSum = 0;
        // We'll store a list of symbols in this column that are unassigned
        std::vector<char> unassignedSymbols;

        // 1) Calculate partial sum from assigned symbols; note any unassigned
        for (auto &pair : columns[colI]) {
            char symbol = pair.first;
            int freq    = pair.second;

            // freq <= 0 means the symbol doesn't appear here
            if (freq <= 0) continue;

            int assignedDigit = decryptMap[symbol];

            if (assignedDigit == -1) {
                // Symbol not yet assigned, record it
                // We only store the symbol freq times conceptually, but to optimize
                // we keep track of each symbol once, and remember 'freq' for sum usage
                for (int i = 0; i < freq; i++) {
                    unassignedSymbols.push_back(symbol);
                }
            } else {
                // Already assigned, add freq * that digit
                columnSum += (assignedDigit * freq);
            }
        }

        // We also must check the "result symbol" for this column,
        // because that too might be unassigned or assigned
        char resultSymbol = results[colI];
        int resultDigit   = decryptMap[resultSymbol];

        // We will handle the result symbol at the end, but keep track
        // if it's unassigned
        bool resultSymbolUnassigned = (resultDigit == -1);

        // 2) Now try to assign digits to unassigned symbols in this column
        // Because unassigned symbols could be multiple, we do a backtracking approach
        // within this function or we can do a small local recursion. For clarity,
        // let's do it in a single approach:

        assignValuesForColumn(unassignedSymbols, 0, colI, carry,
                              columnSum, resultSymbolUnassigned);
    }

    // Helper backtracking to assign digits to all unassigned symbols
    void assignValuesForColumn(
        std::vector<char> &unassignedSymbols,
        int idx,            // index in unassignedSymbols
        int colI,           // current column
        int carry,
        int currentSum,     // partial sum from assigned digits
        bool resultSymbolUnassigned)
    {
        // If we've assigned all unassigned symbols for this column,
        // check whether the sum mod 10 matches the result symbol’s digit
        if (idx == (int)unassignedSymbols.size()) {
            // The sum so far + carry is the total for this column (before mod 10)
            int totalColumnSum = currentSum + carry;

            // Extract the digit that the result symbol must match
            char resultSymbol = results[colI];
            int resultDigit   = decryptMap[resultSymbol];
            
            // If that result symbol is unassigned, we can still try to assign it
            if (resultDigit == -1) {
                // We'll try all digits 0..9 that are not used
                // and see if resultDigit == totalColumnSum % 10
                for (int d = 0; d < 10; d++) {
                    if (!valUsed[d] && d == (totalColumnSum % 10)) {
                        // Assign
                        decryptMap[resultSymbol] = d;
                        valUsed[d] = true;

                        int newCarry = totalColumnSum / 10;
                        // Move to next column
                        solveColumn(colI + 1, newCarry);

                        // Backtrack
                        decryptMap[resultSymbol] = -1;
                        valUsed[d] = false;
                    }
                }
            } else {
                // resultSymbol has a digit. Check if it matches totalColumnSum % 10
                if (resultDigit == (totalColumnSum % 10)) {
                    // If it matches, proceed to next column
                    int newCarry = totalColumnSum / 10;
                    solveColumn(colI + 1, newCarry);
                }
            }
            return;
        }

        // Otherwise, we still have unassigned symbols in this column
        char symbol = unassignedSymbols[idx];
        // If the symbol is already assigned for some reason, skip
        // (In theory shouldn't happen, but let's be defensive)
        if (decryptMap[symbol] != -1) {
            // Just proceed
            assignValuesForColumn(unassignedSymbols, idx + 1, colI,
                                  carry, currentSum, resultSymbolUnassigned);
            return;
        }

        // Try each digit 0..9 for this symbol
        for (int d = 0; d < 10; d++) {
            if (!valUsed[d]) {
                // Assign
                decryptMap[symbol] = d;
                valUsed[d] = true;

                // Add to the sum
                // Notice that we are assigning exactly one frequency at a time
                // per symbol in unassignedSymbols, so each symbol might appear
                // multiple times in unassignedSymbols if freq > 1.
                // So we add 'd' to currentSum (since each unassignedSymbols[idx]
                // corresponds to 1 occurrence).
                int addedSum = currentSum + d;

                // Move on to the next unassigned symbol
                assignValuesForColumn(unassignedSymbols, idx + 1, colI,
                                      carry, addedSum, resultSymbolUnassigned);

                // Backtrack
                decryptMap[symbol] = -1;
                valUsed[d] = false;
            }
        }
    }

public:
    // Constructor
    CryptarithmSolver() {}

    // Build the puzzle columns from the four strings: A, B, C, Res
    // We'll assume they are all the same length for simplicity
    void buildColumns(const std::string &strA,
                      const std::string &strB,
                      const std::string &strC,
                      const std::string &strRes)
    {
        // Clear old data
        columns.clear();
        results.clear();
        decryptMap.clear();
        for (int i = 0; i < 10; i++) {
            valUsed[i] = false;
        }
        currentSolutionsCount = 0;

        int n = strA.size();
        columns.resize(n);
        results.resize(n);

        // Build columns from right to left
        // i=0 is least significant digit, i=n-1 is most significant
        // But you can store them in either order as long as you're consistent
        for (int i = 0; i < n; i++) {
            // For each addend, note the symbol in position (n-1 - i)
            // so that index 0 in columns is the least significant position
            char symA = strA[n - 1 - i];
            char symB = strB[n - 1 - i];
            char symC = strC[n - 1 - i];
            char symR = strRes[n - 1 - i];

            // Increment frequency in columns[i] for A, B, C
            columns[i][symA]++;
            columns[i][symB]++;
            columns[i][symC]++;

            // Store result symbol for this column
            // (which we interpret as the one that should match the sum mod 10)
            results[i] = symR;

            // Also ensure each symbol is in decryptMap with -1 to start
            if (decryptMap.find(symA) == decryptMap.end()) {
                decryptMap[symA] = -1;
            }
            if (decryptMap.find(symB) == decryptMap.end()) {
                decryptMap[symB] = -1;
            }
            if (decryptMap.find(symC) == decryptMap.end()) {
                decryptMap[symC] = -1;
            }
            if (decryptMap.find(symR) == decryptMap.end()) {
                decryptMap[symR] = -1;
            }
        }
    }

    // Solve the puzzle. Returns the number of solutions found.
    int solvePuzzle()
    {
        // Start from column 0 with carry=0
        solveColumn(0, 0);
        return currentSolutionsCount;
    }
};

// --------------------------
// Example usage (main())
// --------------------------
int main()
{
    // Suppose each string has the same length: 3
    // For instance, puzzle:  ABC + BCA + CAB = BBB  (just a random example)
    // The solver will attempt to assign digits to A,B,C so that
    // ABC + BCA + CAB = BBB
    // This example might not have a real solution, but it shows usage.
    CryptarithmSolver solver;
    solver.buildColumns("ABC", "BCA", "CAB", "BBB");
    int numSolutions = solver.solvePuzzle();

    std::cout << "Number of solutions found: " << numSolutions << std::endl;
    return 0;
}
