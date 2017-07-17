#pragma once
#include <vector>

namespace MIDEncryption
{
    class MIDEncryption
    {
    public:
        MIDEncryption(std::vector<uint8_t> mid);
        ~MIDEncryption();

        bool ParseData(std::string &encryptionData, const std::string &signature, std::vector<uint8_t> &decryptionData);

    private:
        std::vector<uint8_t> mid_;
    };
}