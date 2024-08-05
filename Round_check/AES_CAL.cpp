#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
    ifstream inputFile("aes_output.txt");
    ofstream outputFile("aes_Cal.txt", ios::app);

    if (!inputFile.is_open()) {
        cerr << "Error opening input file." << endl;
        return 1;
    }

    if (!outputFile.is_open()) {
        cerr << "Error opening output file." << endl;
        inputFile.close();
        return 1;
    }

    string line;
    int zeroCount = 0;
    int oneCount = 0;

    while (getline(inputFile, line)) {
        for (char c : line) {
            if (c == '0') {
                zeroCount++;
            } else if (c == '1') {
                oneCount++;
            }
        }
    }

    outputFile << "Zeroes: " << zeroCount << ", Ones: " << oneCount << endl;

    inputFile.close();
    outputFile.close();

    cout << "Counting complete. Results appended to aes_Cal.txt" << endl;

    return 0;
}
