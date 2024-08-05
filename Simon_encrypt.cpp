#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include<string.h>

// Function for 64-bit circular bit rotations
template <typename T>
T rotl(T x, unsigned int bits) {
    const unsigned int size_in_bits = sizeof(T) * 8;
    return (x << bits) | (x >> (size_in_bits - bits));
}

template <typename T>
T rotr(T x, unsigned int bits) {
    const unsigned int size_in_bits = sizeof(T) * 8;
    return (x >> bits) | (x << (size_in_bits - bits));
}

// SIMON's 'f' function for round transformation
template <typename T>
T simon_f(T v) {
    return (rotl(v, 1) & rotl(v, 8)) ^ rotl(v, 2);
}

// SIMON-128 round function
template <typename T>
void simon_r2(T& x, T& y, T k1, T k2) {
    y ^= simon_f(x) ^ k1;
    x ^= simon_f(y) ^ k2;
}

// Key expansion for SIMON-128 with 128-bit key and 68 rounds
void simon128_key_expand_2w(std::array<uint64_t, 68>& round_keys, const std::array<uint64_t, 2>& user_key) {
    const uint64_t c = 0xfffffffffffffffc;
    uint64_t z = 0x7369f885192c0ef5;

    round_keys[0] = user_key[1];
    round_keys[1] = user_key[0];

    for (size_t i = 2; i < 68; i++) {
        round_keys[i] = c ^ (z & 1) ^ rotr(round_keys[i - 1], 3) ^ rotr(round_keys[i - 1], 4) ^ round_keys[i - 2];
        z >>= 1;
    }
}

// SIMON-128 encryption function
void simon128_encrypt(std::vector<uint8_t>& data, const std::array<uint64_t, 68>& round_keys) {
    const size_t block_size = 16; // 128 bits = 16 bytes

    for (size_t i = 0; i < data.size(); i += block_size) {
        // Prepare the block for encryption (padded with zeros if necessary)
        std::array<uint8_t, 16> block = {};
        std::copy(data.begin() + i, std::min(data.begin() + i + block_size, data.end()), block.begin());

        // Convert the block into two 64-bit values for SIMON
        uint64_t plaintext[2] = {
            *reinterpret_cast<uint64_t*>(&block[0]),
            *reinterpret_cast<uint64_t*>(&block[8])
        };

        uint64_t ciphertext[2] = {0};

        // Encrypt with SIMON-128 (68 rounds)
        simon_r2(ciphertext[0], ciphertext[1], round_keys[0], round_keys[1]);

        for (size_t j = 2; j < 68; j += 2) {
            simon_r2(ciphertext[0], ciphertext[1], round_keys[j], round_keys[j + 1]);
        }

        // Convert the ciphertext back to bytes
        std::copy(reinterpret_cast<uint8_t*>(&ciphertext[0]), reinterpret_cast<uint8_t*>(&ciphertext[1]), block.begin());
        std::copy(block.begin(), block.end(), data.begin() + i);
    }
}

// Function to read an entire file into a byte vector
std::vector<uint8_t> read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    assert(file.is_open());

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
}

// Function to write a byte vector to a file
void write_file(const std::string& filename, const std::vector<uint8_t>& data) {
    std::ofstream file(filename, std::ios::binary);
    assert(file.is_open());

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

#include <sstream>
#include <iomanip>

std::string byte_to_hex(uint8_t byte)
{
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    return ss.str();
}

int main()
{
    try
    {
        // Read the input data from input.txt
        std::vector<uint8_t> input_data = read_file("input.txt");

        // Set a 128-bit key for SIMON-128 with 68 rounds
        std::array<uint64_t, 68> round_keys;
        std::array<uint64_t, 2> user_key = {0x123456789abcdef0, 0xfedcba9876543210};
        simon128_key_expand_2w(round_keys, user_key);

        // Encrypt the input data
        simon128_encrypt(input_data, round_keys);

        // Write the encrypted data to output_encrypt_high.txt
        write_file("output_simon_encrypt.txt", byte_to_hex(input_data));

        std::cout << "Encryption completed successfully." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}