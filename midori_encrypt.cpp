#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <iomanip>
#include<string.h>

using namespace std;

// Substitution Box (S-Box) for Midori
static const uint8_t SBox[64] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x9C, 0x8B, 0x8C, 0x3E, 0x9A, 0x1A, 0x62, 0x69, 0x95, 0xD0, 0xDF, 0x3B, 0x11, 0xA7, 0x52, 0xDB
    };

// Permutation Box (P-Box) for Midori
static const uint8_t PBox[64] = {
    0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x2A, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20,
    0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18, 0x17, 0x64, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
    0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
    };

// Midori round function
void MidoriRound(uint8_t state[64], uint8_t roundKey[64]) {
    // Add round key
    for (int i = 0; i < 64; ++i) {
        state[i] ^= roundKey[i];
    }

    // S-Box layer
    for (int i = 0; i < 64; ++i) {
        state[i] = SBox[state[i]];
    }

    // P-Box layer
    uint8_t temp[64];
    for (int i = 0; i < 64; ++i) {
        temp[i] = state[PBox[i]];
    }
    std::memcpy(state, temp, 64);
}

// Midori encryption function
void MidoriEncrypt(uint8_t state[64], const uint8_t* roundKeys) {
    // Perform 20 rounds of encryption
    for (int round = 0; round < 20; ++round) {
        MidoriRound(state, const_cast<uint8_t*>(roundKeys) + round * 64);
    }
}

#include <sstream>
#include <iomanip>

string byte_to_hex(uint8_t byte)
{
	std::stringstream ss;
	ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
	return ss.str();
}



int main()
{
    // Open the input file
    std::ifstream inputFile("input.txt", std::ios::binary);
    if (!inputFile)
    {
        std::cerr << "Error: Unable to open input file." << std::endl;
        return 1;
    }

    uint8_t plaintext[64];
    inputFile.read(reinterpret_cast<char *>(plaintext), 64);
    inputFile.close();

    uint8_t roundKeys[20][64];
    // Encrypt the plaintext
    MidoriEncrypt(plaintext, reinterpret_cast<uint8_t *>(roundKeys));

    // Open the file again for writing the ciphertext
    std::ofstream outputFile("output_encrypt_midori.txt", std::ios::binary | std::ios::trunc);
    if (!outputFile)
    {
        std::cerr << "Error: Unable to open output file." << std::endl;
        return 1;
    }

    // Write the ciphertext back to the same file
    outputFile.write(reinterpret_cast<char *>(plaintext), 64);
    outputFile.close();

    std::cout << "Encryption completed and saved to output_encrypt_midori.txt." << std::endl;
    string abd = print_hex(plaintext);
    return 0;
}