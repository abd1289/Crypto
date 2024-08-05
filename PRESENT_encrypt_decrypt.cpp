#include <iostream>
#include <vector>
#include <memory>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <sstream>

// Byte structure consisting of two 4-bit nibbles
struct Byte {
    uint8_t nibble1 : 4;
    uint8_t nibble2 : 4;
};

// S-Box and inverse S-Box
const std::vector<uint8_t> S = {0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2};
const std::vector<uint8_t> invS = {0x5, 0xe, 0xf, 0x8, 0xC, 0x1, 0x2, 0xD, 0xB, 0x4, 0x6, 0x3, 0x0, 0x7, 0x9, 0xA};

// Permutation table
const std::vector<uint8_t> P = {
    0, 16, 32, 48, 1, 17, 33, 49, 2, 18, 34, 50, 3, 19, 35, 51,
    4, 20, 36, 52, 5, 21, 37, 53, 6, 22, 38, 54, 7, 23, 39, 55,
    8, 24, 40, 56, 9, 25, 41, 57, 10, 26, 42, 58, 11, 27, 43, 59,
    12, 28, 44, 60, 13, 29, 45, 61, 14, 30, 46, 62, 15, 31, 47, 63
};

// Converts a Hex string to an array of bytes
std::vector<Byte> fromHexStringToBytes(const std::string& block) {
    std::vector<Byte> bytes(8);
    for (int i = 0; i < 8; ++i) {
        bytes[i].nibble1 = (block[2 * i] >= '0' && block[2 * i] <= '9') ? (block[2 * i] - '0') : (block[2 * i] - 'a' + 10);
        bytes[i].nibble2 = (block[2 * i + 1] >= '0' && block[2 * i + 1] <= '9') ? (block[2 * i + 1] - '0') : (block[2 * i + 1] - 'a' + 10);
    }
    return bytes;
}

// Converts an array of bytes to a 64-bit integer
uint64_t fromBytesToLong(const std::vector<Byte>& bytes) {
    uint64_t result = 0;
    for (int i = 0; i < 8; ++i) {
        result = (result << 4) | (bytes[i].nibble1);
        result = (result << 4) | (bytes[i].nibble2);
    }
    return result;
}

// Converts a Hex string to a 64-bit integer
uint64_t fromHexStringToLong(const std::string& block) {
    uint64_t result = 0;
    for (int i = 0; i < 16; ++i) {
        result = (result << 4) | ((block[i] >= '0' && block[i] <= '9') ? (block[i] - '0') : (block[i] - 'a' + 10));
    }
    return result;
}

// Converts a 64-bit integer to an array of bytes
std::vector<Byte> fromLongToBytes(uint64_t block) {
    std::vector<Byte> bytes(8);
    for (int i = 7; i >= 0; --i) {
        bytes[i].nibble2 = (block >> (2 * (7 - i) * 4)) & 0xF;
        bytes[i].nibble1 = (block >> ((2 * (7 - i) + 1) * 4)) & 0xF;
    }
    return bytes;
}

// Converts a 64-bit integer to a Hex string
std::string fromLongToHexString(uint64_t block) {
    std::ostringstream oss;
    oss << std::setw(16) << std::setfill('0') << std::hex << block;
    return oss.str();
}

// Converts a nibble using the S-Box
uint8_t Sbox(uint8_t input) {
    return S[input];
}

// Converts a nibble using the inverse S-Box
uint8_t inverseSbox(uint8_t input) {
    return invS[input];
}

// Permutation function according to the permutation table
uint64_t permute(uint64_t source) {
    uint64_t permutation = 0;
    for (int i = 0; i < 64; ++i) {
        int distance = 63 - i;
        permutation |= ((source >> distance & 0x1) << (63 - P[i]));
    }
    return permutation;
}

// Inverse permutation according to the permutation table
uint64_t inversepermute(uint64_t source) {
    uint64_t permutation = 0;
    for (int i = 0; i < 64; ++i) {
        int distance = 63 - P[i];
        permutation = (permutation << 1) | ((source >> distance) & 0x1);
    }
    return permutation;
}

// Gets the low 16 bits of the key from a Hex string
uint16_t getKeyLow(const std::string& key) {
    uint16_t keyLow = 0;
    for (int i = 16; i < 20; ++i) {
        keyLow = (keyLow << 4) | ((key[i] >= '0' && key[i] <= '9') ? (key[i] - '0') : (key[i] - 'a' + 10));
    }
    return keyLow;
}

// Generates subkeys from a key according to the PRESENT key scheduling algorithm for an 80-bit key
std::unique_ptr<std::vector<uint64_t>> generateSubkeys(const std::string& key) {
    uint64_t keyHigh = fromHexStringToLong(key);
    uint16_t keyLow = getKeyLow(key);
    
    auto subKeys = std::make_unique<std::vector<uint64_t>>(32);
    (*subKeys)[0] = keyHigh;
    
    for (int i = 1; i < 32; ++i) {
        // Shifting the whole key 61 bits to the left
        uint64_t temp1 = keyHigh;
        uint16_t temp2 = keyLow;
        
        keyHigh = (keyHigh << 61) | (temp2 << 45) | (temp1 >> 19);
        keyLow = (temp1 >> 3) & 0xFFFF;
        
        // The most significant nibble of the key goes through the S-Box
        uint8_t temp = Sbox(keyHigh >> 60);
        keyHigh = (keyHigh & 0x0FFFFFFFFFFFFFFFLL) | (temp << 60);
        
        // Key bits on positions k19, k18, k17, k16, and k15 XORed with round counter
        keyLow ^= ((i & 0x01) << 15);
        keyHigh ^= (i >> 1);
        
        (*subKeys)[i] = keyHigh;
    }
    
    return subKeys;
}

// Function for encrypting a block using a key
std::string encrypt(const std::string& plaintext, const std::string& key) {
    auto subkeys = generateSubkeys(key);
    uint64_t state = fromHexStringToLong(plaintext);
    
    for (int i = 0; i < 31; ++i) {
        state ^= (*subkeys)[i];
        
        auto stateBytes = fromLongToBytes(state);
        for (int j = 0; j < 8; ++j) {
            stateBytes[j].nibble1 = Sbox(stateBytes[j].nibble1);
            stateBytes[j].nibble2 = Sbox(stateBytes[j].nibble2);
        }
        
        state = permute(fromBytesToLong(stateBytes));
    }
    
    state ^= (*subkeys)[31];
    
    return fromLongToHexString(state);
}

// Function for decrypting a block using a key
std::string decrypt(const std::string& ciphertext, const std::string& key) {
    auto subkeys = generateSubkeys(key);
    uint64_t state = fromHexStringToLong(ciphertext);
    
    for (int i = 0; i < 31; ++i) {
        state ^= (*subkeys)[31 - i];
        state = inversepermute(state);
        
        auto stateBytes = fromLongToBytes(state);
        for (int j = 0; j < 8; ++j) {
            stateBytes[j].nibble1 = inverseSbox(stateBytes[j].nibble1);
            stateBytes[j].nibble2 = inverseSbox(stateBytes[j].nibble2);
        }
        
        state = fromBytesToLong(stateBytes);
    }
    
    state ^= (*subkeys)[0];
    
    return fromLongToHexString(state);
}

// Main test function
int main() {
    std::string plaintext;
    std::string key;
    
    std::cout << "Enter the plaintext (64 bits) in hexadecimal format\nUse lower case characters:\n";
    std::getline(std::cin, plaintext);
    
    std::cout << "Enter the key (80 bits) in hexadecimal format\nUse lower case characters:\n";
    std::getline(std::cin, key);
    
    auto ciphertext = encrypt(plaintext, key);
    std::cout << "The ciphertext is: " << ciphertext << "\n";
    
    auto decryptedText = decrypt(ciphertext, key);
    std::cout << "The decrypted plaintext is: " << decryptedText << "\n";
    
    return 0;
}