#pragma once
#include <vector>

namespace MIDEncryption
{
    namespace OpenSSL
    {
        class AESEncryption
        {
        public:
            static void Encrypt(std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
            static void Decrypt(std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
        };
    }

    namespace CryptoAPI
    {
        class AESEncryption
        {
        public:
            static void CBCEncrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
            static void CBCDecrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
        };
    }

    namespace NextCryptoAPI
    {
        class AESEncryption
        {
        public:
            static void CBCEncrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
        };
    }
}

