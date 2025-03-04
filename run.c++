// Try memory reserving stuff
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "crypt.c++"
#include "util/equation_gen.c++"

using json = nlohmann::json;
using namespace std;

namespace std {
    template<>
    struct hash<array<string, 4>> {
        size_t operator()(const array<string, 4>& arr) const {
            size_t hash = 0;
            for (int i = 0; i < 4; i++) {
                // Use std::hash<string> instead of std::hash<int>
                hash ^= std::hash<string>{}(arr[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
    };
}

int main() {
    int display_code = -1;

    bool replace_data = true;
    int num_equations = 100000;
    int num_digits = 3;


    ofstream data_file;
    if (replace_data) {data_file = ofstream("outputs/data.jsonl", ios::trunc);} 
    else {data_file = ofstream("outputs/data.jsonl", ios::app);}

    if (!data_file || !data_file.is_open()) {
        cerr << "Error opening output file(s)." << endl;
        return 1;
    }


    ofstream output_file("outputs/output.txt", ios::out | ios::trunc);
    if (!output_file.is_open()) {
        cout << "Error opening out file" << endl;
        return 1;
    }
    cout.rdbuf(output_file.rdbuf());


    unordered_map<array<string, 4>, int> equationNumberSolutions;
    unordered_map<string, int> equationSolutions;


    EquationGenerator generator(num_digits, display_code);
    CryptarithmSolver solver(num_digits, display_code);
    for (int t = 0; t < num_equations; t++) {
        // Equation Generator
        generator.generateEncryptedEquation();

        // Solver
        array<vector<int>, 4> equation = generator.encryptedEquationRet;
        int solutions = solver.solve(equation[0], equation[1], equation[2], equation[3]);

        // Data
        json data;
        data["encrypted_equation_str"] = generator.encryptedEquationFullStrRet; // Encrypted string for generating question
        data["encrypted_equation"] = generator.encryptedEquationStrRet; // Encrypted list of strings for checking AI solution matches encrypted equation
        data["num_solutions"] = solutions;
        data_file << data.dump() << endl;
        
        // Logging
        string equationStr = generator.encryptedEquationFullStrRet;
        equationSolutions[equationStr] = solutions;
        if (display_code >= 1) {cout << generator.equationStr << endl;}
    }
    data_file.close();
    
    // Logging
    cout << "Equation Solutions:\n";
    for (auto& equa_sol_pair : equationSolutions) {
        cout << equa_sol_pair.first << " -> " << equa_sol_pair.second << endl;
        clog << equa_sol_pair.first << " -> " << equa_sol_pair.second << endl;
    }

    output_file.close();

    clog << "TASK END" << endl;
    return 0;
}