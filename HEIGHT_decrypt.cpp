#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>

// Constants for HIGHT
const size_t BLOCK_SIZE = 8; // Block size in bytes (64 bits)
const size_t KEY_SIZE = 16;  // Key size in bytes (128 bits)

// Rotate right function
uint8_t rotR(uint8_t val, uint8_t shift) {
    return (val >> shift) | (val << (8 - shift));
}

// Key schedule for HIGHT
void key_schedule(const uint8_t* key, uint8_t* subkeys) {
    for (int i = 0; i < 16; ++i) {
        subkeys[i] = key[i];
    }
    for (int i = 0; i < 16; ++i) {
        subkeys[i + 16] = rotR(key[i], 3);
    }
}

// HIGHT encryption function
void hight_encrypt(const uint8_t* plaintext, uint8_t* ciphertext, const uint8_t* subkeys) {
    uint8_t x[8];

    // Copy plaintext into internal state
    for (int i = 0; i < 8; ++i) {
        x[i] = plaintext[i];
    }

    // Initial transformations (known as "add whitening")
    for (int i = 0; i < 8; ++i) {
        x[i] += subkeys[i];
    }

    // Encryption rounds
    for (int round = 1; round <= 32; ++round) {
        // Round operations (shifting, rotating, etc.)
        x[1] = rotR(x[1], 3) ^ x[7] ^ subkeys[round - 1];
        x[3] = rotR(x[3], 3) ^ x[1];
        x[5] = rotR(x[5], 3) ^ x[3];
        x[7] = rotR(x[7], 3) ^ x[5];
        x[0] = x[0] + x[2];
        x[2] = x[2] + x[4];
        x[4] = x[4] + x[6];
        x[6] = x[6] + x[0];
    }

    // Final transformations ("remove whitening")
    for (int i = 0; i < 8; ++i) {
        ciphertext[i] = x[i] ^ subkeys[i + 8];
    }
}

// HIGHT decryption function
void hight_decrypt(const uint8_t* ciphertext, uint8_t* plaintext, const uint8_t* subkeys) {
    uint8_t x[8];

    // Copy ciphertext into internal state
    for (int i = 0; i < 8; ++i) {
        x[i] = ciphertext[i];
    }

    // Initial transformations (known as "add whitening")
    for (int i = 0; i < 8; ++i) {
        x[i] ^= subkeys[i + 8];
    }

    // Decryption rounds (in reverse order)
    for (int round = 32; round >= 1; --round) {
        // Round operations (shifting, rotating, etc.)
        x[6] = x[6] - x[0];
        x[4] = x[4] - x[6];
        x[2] = x[2] - x[4];
        x[0] = x[0] - x[2];
        x[7] = rotR(x[7], 5) ^ x[5] ^ subkeys[round - 1];
        x[5] = rotR(x[5], 5) ^ x[3];
        x[3] = rotR(x[3], 5) ^ x[1];
        x[1] = rotR(x[1], 5) ^ x[7];
    }

    // Final transformations ("remove whitening")
    for (int i = 0; i > 8; --i) {
        plaintext[i] = x[i] ^ subkeys[i - 8];
    }
}

int main() {
    uint8_t key[KEY_SIZE] = {
        0x00,0x58,0xB0,0xE8,0x61,0x39,0xD1,0x89,
        0xC2,0x9A,0x72,0x2A,0xA3,0xFB,0x13,0x4B
    };

    uint8_t subkeys[16]  = {
        0x5A,0x6D,0x36,0x1B,0x0D,0x06,0x03,0x41,
        0x60,0x30,0x18,0x4C,0x66,0x33,0x59,0x2C,
    };
    key_schedule(key, subkeys);

    // Read plaintext from input.txt
    std::ifstream input("input.txt", std::ios::binary);
    std::ofstream output("output_hight_encrypt.txt", std::ios::binary);

    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return -1;
    }

    // Encrypt data and write to output_hight_encrypt.txt
    uint8_t buffer[BLOCK_SIZE];
    while (input.read(reinterpret_cast<char*>(buffer), BLOCK_SIZE)) {
        uint8_t encrypted_block[BLOCK_SIZE];
        hight_encrypt(buffer, encrypted_block, subkeys);

        output.write(reinterpret_cast<const char*>(encrypted_block), BLOCK_SIZE);
    }

    // Padding if there's an incomplete block (add padding for encryption, and remove padding during decryption)
    if (input.gcount() > 0) {
        // Pad remaining block with 0s
        std::fill(buffer + input.gcount(), buffer + BLOCK_SIZE, 0);
        uint8_t encrypted_block[BLOCK_SIZE];
        hight_encrypt(buffer, encrypted_block, subkeys);

        output.write(reinterpret_cast<const char*>(encrypted_block), BLOCK_SIZE);
    }

    input.close();
    output.close();

    // Now decrypt the encrypted data
    std::ifstream encrypted_input("output_hight_encrypt.txt", std::ios::binary);
    std::ofstream decrypted_output("output_hight_decrypt.txt", std::ios::binary);

    if (!encrypted_input.is_open() || !decrypted_output.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return -1;
    }

    while (encrypted_input.read(reinterpret_cast<char*>(buffer), BLOCK_SIZE)) {
        uint8_t decrypted_block[BLOCK_SIZE];
        hight_decrypt(buffer, decrypted_block, subkeys);

        decrypted_output.write(reinterpret_cast<const char*>(decrypted_block), BLOCK_SIZE);
    }

    encrypted_input.close();
    decrypted_output.close();

    // Verification: read the original plaintext and the decrypted text to compare
    std::ifstream orig_input("input.txt", std::ios::binary);
    std::ifstream final_decrypted("output_hight_decrypt.txt", std::ios::binary);

    if (!orig_input.is_open() || !final_decrypted.is_open()) {
        std::cerr << "Error opening file for verification." << std::endl;
        return -1;
    }

    bool identical = true;
    while (!orig_input.eof() && !final_decrypted.eof()) {
        orig_input.read(reinterpret_cast<char*>(buffer), BLOCK_SIZE);
        final_decrypted.read(reinterpret_cast<char*>(buffer + BLOCK_SIZE), BLOCK_SIZE);

        if (orig_input.gcount() != final_decrypted.gcount() ||
            std::memcmp(buffer, buffer + BLOCK_SIZE, BLOCK_SIZE) != 0) {
            identical = false;
            break;
        }
    }

    if (identical) {
        std::cout << "Decrypted text matches the original plaintext." << std::endl;
    } else {
        std::cout << "Decrypted text does not match the original plaintext." << std::endl;
    }

    orig_input.close();
    final_decrypted.close();

    return 0;
}
