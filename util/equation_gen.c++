#include <iostream>
#include <random>
using namespace std;

class EquationGenerator {
private:
    int numDigits;

    array<int, 10> generateEncryptMap() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distribSolution(0, 25);
    
        array<int, 10> encryptMap; // Value to symbol
        fill(encryptMap.begin(), encryptMap.end(), -1);

        array<bool, 26> symbolUsed;
        fill(symbolUsed.begin(), symbolUsed.end(), false);
    
        for (int val = 0; val < 10; val++) {
            int offset = distribSolution(gen);
            while (symbolUsed[offset]) {offset = distribSolution(gen);}
            
            encryptMap[val] = offset;
            symbolUsed[offset] = true;
        }
    
        return encryptMap;
    }
    
    array<vector<int>, 4> generateEquation() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distrib(pow(10, numDigits - 2), pow(10, numDigits - 1) - 1);
    
        // Generating Numbers
        int a, b, c; int r = 0;
        while (r < pow(10, numDigits - 1)) {
            a = distrib(gen);
            b = distrib(gen);
            c = distrib(gen);
            r = a + b + c;
        }

        // REMOVE: Testing
        // a = 7703; b = 9849; c = 8009;
        // r = a + b + c;
        // REMOV: Testing

        equationStr = to_string(a) + " + " + to_string(b) + " + " + to_string(c) + " = " + to_string(r);

        // Formatting Numbers - Last value (First digit) in A, B, C will always be 0
        vector<int> A;
        vector<int> B;
        vector<int> C;
        vector<int> R;
    
        for (int i = 0; i < numDigits; i++) {
            A.push_back(a % 10);
            B.push_back(b % 10);
            C.push_back(c % 10);
            R.push_back(r % 10);
    
            a /= 10; b /= 10; c /= 10; r /= 10;
        }
    
        array<vector<int>, 4> equation;
        equation[0] = A;
        equation[1] = B;
        equation[2] = C;
        equation[3] = R;

        return equation;
    }

public:
    array<vector<int>, 4> encryptedEquationRet;
    array<string, 4> encryptedEquationStrRet;
    string encryptedEquationFullStrRet;

    string equationStr;

    EquationGenerator(int digits, int display_code): numDigits(digits + 1) 
    {}

    void generateEncryptedEquation() {
        array<int, 10> encryptMap = generateEncryptMap();
        array<vector<int>, 4> equation = generateEquation();

        array<vector<int>, 4> encryptedEquationSol; // For solver
        array<string, 4> encryptedEquationData; // For data

        for (int enI = 0; enI < 4; enI++) {
            vector<int> enNum;
            string enNumStr = "";

            for (int i = 0; i < numDigits - 1; i++) {
                int offset = encryptMap[equation[enI][i]];
                enNum.push_back(offset);
                enNumStr += 'A' + offset;
            }

            if (equation[enI][numDigits - 1] != 0) {
                int offset = encryptMap[equation[enI][numDigits - 1]];
                enNum.push_back(offset);
                enNumStr += 'A' + offset;
            } else {
                enNum.push_back(-1);
            }

            encryptedEquationSol[enI] = enNum;
            encryptedEquationData[enI] = enNumStr;
        }

        encryptedEquationRet = encryptedEquationSol;
        encryptedEquationStrRet = encryptedEquationData;
        encryptedEquationFullStrRet = encryptedEquationData[0] + " + " + encryptedEquationData[1] + " + " + encryptedEquationData[2] + " = " + encryptedEquationData[3];
    }
};

// int main() {
//     int num_digits = 4;
//     int display_code = 2; // 0 - No display, 1 - Minumal display, 2 - All display

//     EquationGenerator encryptor(num_digits, display_code);
//     array<vector<int>, 4> equation = encryptor.generateEncryptedEquation();

//     switch (display_code) {
//         case 0:
//             break;
//         case 1:
//             cout << "\nEncrypted Equation (symbols):\n";
//             for (int i = 0; i < equation[0].size(); i++) {cout << equation[0][i] << " ";} cout << "\n";
//             for (int i = 0; i < equation[1].size(); i++) {cout << equation[1][i] << " ";} cout << "\n";
//             for (int i = 0; i < equation[2].size(); i++) {cout << equation[2][i] << " ";} cout << "\n";
//             for (int i = 0; i < equation[3].size(); i++) {cout << equation[3][i] << " ";} cout << endl;
//             break;
//         case 2:
//             encryptor.display();
//             break;
//     }

//     cout << "Task End" << endl;
//     return 0;
// }