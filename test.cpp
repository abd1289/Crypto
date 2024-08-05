#include <iostream>
#include <vector>
#include <algorithm> // for std::shuffle
#include <random>   // for std::mt19937, std::random_device

std::pair<std::vector<uint8_t>, std::vector<uint8_t>> createSAndInvS() {
    std::vector<uint8_t> S(16);
    std::vector<uint8_t> invS(16);

    // Fill S with numbers 0 to 15
    std::iota(S.begin(), S.end(), 0);

    // Shuffle S to make it random
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(S.begin(), S.end(), g);

    // Create inverse mapping
    for (size_t i = 0; i < S.size(); ++i) {
        invS[S[i]] = i;
    }

    return {S, invS};
}

int main()
{
    std::pair<std::vector<uint8_t>, std::vector<uint8_t>> result = createSAndInvS();
    auto &S = result.first;
    auto &invS = result.second;

    // Print S box
    std::cout << "S box: ";
    for (auto s : S)
    {
        std::cout << std::hex << int(s) << " ";
    }
    std::cout << std::endl;

    // Print invS box
    std::cout << "invS box: ";
    for (auto s : invS)
    {
        std::cout << std::hex << int(s) << " ";
    }
    std::cout << std::endl;

    return 0;
}