#include <iostream>
#include <random>
using namespace std;

class Encryptor {
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
            int symbol = distribSolution(gen);
            while (symbolUsed[symbol]) {symbol = distribSolution(gen);}
            
            encryptMap[val] = symbol;
            symbolUsed[symbol] = true;
        }
    
        encryptMapStore = encryptMap;
        return encryptMap;
    }
    
    array<vector<int>, 4> generateEquation() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distrib(pow(10, numDigits - 1), pow(10, numDigits) - 1);
    
        // Generating Numbers
        int a = distrib(gen);
        int b = distrib(gen);
        int c = distrib(gen);
        int r = a + b + c;
        
        array<int, 4> equationNumbers;
        equationNumbers[0] = a;
        equationNumbers[1] = b;
        equationNumbers[2] = c;
        equationNumbers[3] = r;

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
    

        equationStore = equation;
        equationNumbersStore = equationNumbers;
        return equation;
    }

public:
    array<int, 10> encryptMapStore;
    array<int, 4> equationNumbersStore;
    array<vector<int>, 4> equationStore;
    array<vector<int>, 4> encryptedEquationStore;

    Encryptor(int digits): numDigits(digits + 1) 
    {}

    array<vector<int>, 4> generateEncryptedEquation() {
        array<int, 10> encryptMap = generateEncryptMap();
        array<vector<int>, 4> equation = generateEquation();

        vector<int> enA;
        vector<int> enB;
        vector<int> enC;
        vector<int> enR;

        for (int i = 0; i < numDigits - 1; i++) {
            enA.push_back(encryptMap[equation[0][i]]);
            enB.push_back(encryptMap[equation[1][i]]);
            enC.push_back(encryptMap[equation[2][i]]);
            enR.push_back(encryptMap[equation[3][i]]);
            enR.push_back(encryptMap[equation[4][i]]);
        }
        if (equation[0][numDigits - 1] != 0) {enA.push_back(encryptMap[equation[0][numDigits - 1]]);}
        else {enA.push_back(-1);}
        if (equation[1][numDigits - 1] != 0) {enB.push_back(encryptMap[equation[0][numDigits - 1]]);}
        else {enB.push_back(-1);}
        if (equation[2][numDigits - 1] != 0) {enC.push_back(encryptMap[equation[0][numDigits - 1]]);}
        else {enC.push_back(-1);}
        if (equation[3][numDigits - 1] != 0) {enR.push_back(encryptMap[equation[0][numDigits - 1]]);}
        else {enR.push_back(-1);}


        array<vector<int>, 4> encryptedEquation;
        encryptedEquation[0] = enA;
        encryptedEquation[1] = enB;
        encryptedEquation[2] = enC;
        encryptedEquation[3] = enR;

        encryptedEquationStore = encryptedEquation;
        return encryptedEquation;
    }

    void display() const {
        cout << "Encrypt Map (Value -> Symbol):\n";
        for (int i = 0; i < 10; i++) {cout << i << " -> " << encryptMapStore[i] << "\n";}
        cout << endl;

        cout << "\nEquation Numbers:\n";
        cout << equationNumbersStore[0] << " + " 
             << equationNumbersStore[1] << " + " 
             << equationNumbersStore[2] << " = " 
             << equationNumbersStore[3] << endl;

        cout << "\nOriginal Equation (digits):\n";
        for (const auto& num : equationStore[0]) { cout << num << " "; } cout << "\n";
        for (const auto& num : equationStore[1]) { cout << num << " "; } cout << "\n";
        for (const auto& num : equationStore[2]) { cout << num << " "; } cout << "\n";
        for (const auto& num : equationStore[3]) { cout << num << " "; } cout << endl;

        cout << "\nEncrypted Equation (symbols):\n";
        for (const auto& num : encryptedEquationStore[0]) { cout << num << " "; } cout << "\n";
        for (const auto& num : encryptedEquationStore[1]) { cout << num << " "; } cout << "\n";
        for (const auto& num : encryptedEquationStore[2]) { cout << num << " "; } cout << "\n";
        for (const auto& num : encryptedEquationStore[3]) { cout << num << " "; } cout << endl;
    }
};

int main() {
    int numDigits = 10;
    int displayCode = 0; // 0 - No display, 1 - Minumal display, 2 - All display

    Encryptor encryptor(numDigits);
    array<vector<int>, 4> equation = encryptor.generateEncryptedEquation();

    switch (displayCode) {
        case 0:
            break;
        case 1:
            cout << "\nEncrypted Equation (symbols):\n";
            for (int i = 0; i < equation[0].size(); i++) {cout << equation[0][i] << " ";} cout << "\n";
            for (int i = 0; i < equation[1].size(); i++) {cout << equation[1][i] << " ";} cout << "\n";
            for (int i = 0; i < equation[2].size(); i++) {cout << equation[2][i] << " ";} cout << "\n";
            for (int i = 0; i < equation[3].size(); i++) {cout << equation[3][i] << " ";} cout << endl;
            break;
        case 2:
            encryptor.display();
            break;
    }

    cout << "Task End" << endl;
    return 0;
}