// Try memory reserving stuff
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "crypt.c++"
#include "util/equation_gen.c++"

using json = nlohmann::json;
using namespace std;

int main() {
    int display_code = -1;

    int num_equations = 100000;
    int num_digits = 4;
    bool replace_data = true;

    ofstream data_file;
    if (replace_data) {data_file = ofstream("outputs/data.jsonl", ios::trunc);} 
    else {data_file = ofstream("outputs/data.jsonl", ios::app);}

    if(!data_file || !data_file.is_open()){
        cerr << "Error opening output file(s)." << endl;
        return 1;
    }

    ofstream output_file("outputs/output.txt", ios::out | ios::trunc);
    if (!output_file.is_open()) {
        cout << "Error opening out file" << endl;
        return 1;
    }
    cout.rdbuf(output_file.rdbuf());

    unordered_map<string, int> equationSolutions;

    EquationGenerator generator(num_digits, display_code);
    CryptarithmSolver solver(num_digits, display_code);

    for (int t = 0; t < num_equations; t++) {
        array<vector<int>, 4> equation = generator.generateEncryptedEquation();

        if (display_code >= 1) {cout << generator.equation << endl;}

        int solutions = solver.solve(equation[0], equation[1], equation[2], equation[3]);

        string equationStr = generator.equation;
        equationSolutions[equationStr] = solutions;
    }

    cout << "Equation Solutions:\n";
    for (auto& equa_sol_pair : equationSolutions) {
        json data;
        data["equation"] = equa_sol_pair.first;
        data["num_solutions"] = equa_sol_pair.second;

        data_file << data.dump() << endl;
        cout << equa_sol_pair.first << " -> " << equa_sol_pair.second << endl;
        clog << equa_sol_pair.first << " -> " << equa_sol_pair.second << endl;
    }

    data_file.close();
    output_file.close();

    clog << "TASK END" << endl;
    return 0;
}