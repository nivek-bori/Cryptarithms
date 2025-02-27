#include <iostream>
#include "crypt.c++"
#include "util/equation_gen.c++"

int main() {
    int num_equations = 1;
    int num_digits = 1; // This is the result's # digits, num_digits - 1 is the number's # digits

    unordered_map<string, int> equationSolutions;

    EquationGenerator generator(num_digits);
    CryptarithmSolver solver(num_digits, 2);

    for (int t = 0; t < num_equations; t++) {
        array<vector<int>, 4> equation = generator.generateEncryptedEquation();
        int solutions = solver.solve(equation[0], equation[1], equation[2], equation[3]);

        string equationStr = generator.equation;
        equationSolutions[equationStr] = solutions;
    }

    cout << "Equation Solutions:\n";
    for (auto& equa_sol_pair : equationSolutions) {
        cout << equa_sol_pair.first << " -> " << equa_sol_pair.second << endl;
    }

    cout << "TASK END" << endl;
    return 0;
}