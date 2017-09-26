#pragma once
#include <vector>
#include <cassert>

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
            static void CBC128Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
            static void CBC128Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);

            static void CBC256Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
            static void CBC256Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
        };

        class RC4Encryption
        {
        public:
            static void Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
            static void Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
        };
    }

    namespace NextCryptoAPI
    {
        class Key
        {
            // See also http://msdn.microsoft.com/en-us/library/aa379916(v=vs.85).aspx remarks

        public:
            typedef void(*HashHanlder)(const std::vector<uint8_t> &, std::vector<uint8_t> &);

            template <HashHanlder hashHandler, int bits = 16>
            static void CreateKey(const std::vector<uint8_t> &inputKey, std::vector<uint8_t> &outputKey) {
                std::vector<uint8_t> keyBuffer;
                hashHandler(inputKey, keyBuffer);
                
                assert(bits == keyBuffer.size() && outputKey.size() <= 2 * bits);

                std::vector<uint8_t> buffer1(64, 0x36);
                for (int index = 0; index < keyBuffer.size(); ++index) {
                    buffer1[index] ^= keyBuffer[index];
                }

                std::vector<uint8_t> buffer2(64, 0x5C);
                for (int index = 0; index < keyBuffer.size(); ++index) {
                    buffer2[index] ^= keyBuffer[index];
                }

                std::vector<uint8_t> buffer1Hash;
                hashHandler(buffer1, buffer1Hash);
                std::vector<uint8_t> buffer2Hash;
                hashHandler(buffer2, buffer2Hash);

                int index = 0;
                for (; index < outputKey.size() && index < bits; ++index) {
                    outputKey[index] = buffer1Hash[index];
                }

                for (; index < outputKey.size() && index < 2 * bits; ++index) {
                    outputKey[index] = buffer2Hash[index - bits];
                }
            }
        };

        class AESEncryption
        {
        public:
            static void CBC128Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
            static void CBC128Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);

            static void CBC256Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
            static void CBC256Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv);
        };

        class RC4Encryption
        {
        public:
            static void Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key);
            static void Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key);
        };
    }
}

