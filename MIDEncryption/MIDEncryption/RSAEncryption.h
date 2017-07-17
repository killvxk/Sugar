#pragma once
#include <vector>

namespace MIDEncryption
{
    class RSAEncryption
    {
    public:
        static void PublicEncrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &publicKey);
        static void PrivateDecrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &privateKey);
        static void PrivateEncrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &privateKey);
        static void PublicDecrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &publicKey);
        static bool VerifySignature(const std::vector<uint8_t> &dataBuffer, const std::vector<uint8_t> &signatureData, const std::string &publicKey);
    };
}