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
            static void Sha256(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
            static void MD5(std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);

        private:
            static void doHash(ALG_ID Algid, const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
        };
    }

    namespace NextCryptoAPI
    {
        class Hash
        {
        public:
            static void Sha256(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
            static void MD5(std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);

        private:
            static void doHash(LPCWSTR pszAlgId, const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer);
        };
    }
}

