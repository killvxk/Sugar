#pragma once
#include <vector>
#include <string>
#include <Windows.h>

namespace MIDEncryption
{
    namespace CryptoAPI
    {
        class Hash
        {
        public:
            static void Sha1(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
            static void Sha256(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
            static void Sha512(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
            static void MD5(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);

        private:
            static void hash(ALG_ID Algid, const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
        };
    }

    namespace NextCryptoAPI
    {
        class Hash
        {
        public:
            static void Sha1(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
            static void Sha256(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
            static void Sha512(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
            static void MD5(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);

        private:
            static void hash(LPCWSTR pszAlgId, const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
        };
    }
}

