#include <iostream>
#include <cstdlib>

int main() {
    // Define the paths to the executable files
    std::string aesPath = "AES_encrypt.exe";
    std::string blowfishPath = "Blowfish_encrypt.exe";
    std::string hightPath = "HIGHT_encrypt.exe";

    std::cout << "Project By CipherCrafters: Encryption Squad\n";
    std::cout << "Select the encryption program to run:\n";
    std::cout << "1) AES Encryption\n";
    std::cout << "2) Blowfish Encryption\n";
    std::cout << "3) HIGHT Encryption\n";
    std::cout << "4) RUN All Encryption\n";
    std::cout << "Enter the number of your choice: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1:
            std::cout << "Running AES encryption...\n";
            std::system(aesPath.c_str());
            break;
        case 2:
            std::cout << "Running Blowfish encryption...\n";
            std::system(blowfishPath.c_str());
            break;
        case 3:
            std::cout << "Running HIGHT encryption...\n";
            std::system(hightPath.c_str());
            break;
        case 4:
            std::cout << "Running ALL encryption...\n";
            std::system(hightPath.c_str());
            std::system(blowfishPath.c_str());
            std::system(aesPath.c_str());
            break;
        default:
            std::cout << "Invalid choice. Exiting.\n";
            break;
    }

    return 0;
}
