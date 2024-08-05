#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
    ifstream inputFile("output_hight_encrypt_round.txt");
    ofstream outputFile("hight_Cal.txt", ios::app);

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
    int totalBits = 0;

    while (getline(inputFile, line)) {
        for (char c : line) {
            if (c == '0') {
                zeroCount++;
            } else if (c == '1') {
                oneCount++;
            }
        }
    }

    totalBits = zeroCount + oneCount;
    double zeroPercentage = (static_cast<double>(zeroCount) / totalBits) * 100;
    double onePercentage = (static_cast<double>(oneCount) / totalBits) * 100;

    outputFile << "Zeroes: " << zeroCount << " (" << zeroPercentage << "%), "
               << "Ones: " << oneCount << " (" << onePercentage << "%)" << endl;

    inputFile.close();
    outputFile.close();

    cout << "Counting complete. Results appended to hight_Cal.txt" << endl;

    return 0;
}