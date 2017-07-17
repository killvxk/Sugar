#pragma once
#include <vector>
#include <string>

namespace MIDEncryption
{
    class Sha
    {
    public:
        static void Hash256(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
    };
}

