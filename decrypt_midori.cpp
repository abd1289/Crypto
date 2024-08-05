// midori.cpp

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>

// Constants
const int BLOCK_SIZE = 8;
const int NUM_ROUNDS = 20;

// Substitution Box (S-Box) for Midori
static const uint8_t s_box[BLOCK_SIZE] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75
};

// Permutation Box (P-Box) for Midori
static const uint8_t p_box[BLOCK_SIZE]= {
    0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x2A, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20,
    0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18, 0x17, 0x64, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
    0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};

// Apply inverse S-Box
void apply_inverse_s_box(uint8_t state[BLOCK_SIZE])
{
    static const uint8_t inverse_s_box[16] = {
        0x5, 0xB, 0x1, 0xE, 0xA, 0x7, 0x9, 0xF,
        0x0, 0x4, 0xD, 0x6, 0x3, 0x8, 0x2, 0xC};

    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        state[i] = inverse_s_box[state[i] & 0x0F];
    }
}

// Midori round function (inverse)
void midori_round_inv(uint8_t state[BLOCK_SIZE], const uint8_t *round_key)
{
    midori_inv_p_box(state);
    apply_inverse_s_box(state);
    midori_inv_add_round_key(state, round_key);
}


// Midori decryption function
void midori_decrypt(uint8_t state[BLOCK_SIZE], const uint8_t* round_keys) {
    for (int round = NUM_ROUNDS - 1; round >= 0; --round) {
        midori_round_inv(state, round_keys + round * BLOCK_SIZE);
    }
 
int main()
    {
        // Open the input file
        std::ifstream input_file("input.txt", std::ios::binary);
        if (!input_file)
        {
            std::cerr << "Error: Unable to open input file." << std::endl;
            return 1;
        }

        // Read the plaintext from the input file
        uint8_t plaintext[BLOCK_SIZE];
        input_file.read(reinterpret_cast<char *>(plaintext), BLOCK_SIZE);
        input_file.close();

        // Example round keys (assuming pre-generated)
        uint8_t round_keys[NUM_ROUNDS][BLOCK_SIZE] = {}; // Assume round keys are pre-generated

        // Encrypt the plaintext
        midori_encrypt(plaintext, round_keys);

        // Open the file again for writing the ciphertext
        std::ofstream output_file("output_encrypt_midori.txt", std::ios::binary | std::ios::trunc);
        if (!output_file)
        {
            std::cerr << "Error: Unable to open output file." << std::endl;
            return 1;
        }

        // Write the ciphertext back to the same file
        output_file.write(reinterpret_cast<char *>(plaintext), BLOCK_SIZE);
        output_file.close();

        std::cout << "Encryption completed and saved to output_encrypt_midori.txt." << std::endl;

        // Decrypt the ciphertext
        uint8_t decrypted_text[BLOCK_SIZE];
        memcpy(decrypted_text, plaintext, BLOCK_SIZE);
        midori_decrypt(decrypted_text, round_keys[0]);

        // Open the file again for writing the decrypted text
        std::ofstream decrypted_file("output_decrypt_midori.txt", std::ios::binary | std::ios::trunc);
        if (!decrypted_file)
        {
            std::cerr << "Error: Unable to open output file." << std::endl;
            return 1;
        }

        // Write the decrypted text back to the same file
        decrypted_file.write(reinterpret_cast<char *>(decrypted_text), BLOCK_SIZE);
        decrypted_file.close();

        std::cout << "Decryption completed and saved to output_decrypt_midori.txt." << std::endl;

        return 0;
    }

// Apply P-Box
void apply_p_box(uint8_t state[BLOCK_SIZE])
{
    uint8_t temp[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        temp[p_box[i]] = state[i];
    }
    memcpy(state, temp, BLOCK_SIZE);
}