#include "stdafx.h"
#include "MIDEncryption.h"
#include "AESEncryption.h"
#include "Hash.h"
#include "RSAEncryption.h"

namespace MIDEncryption
{
    std::string PUBLICKEY = "-----BEGIN PUBLIC KEY-----\n"\
        "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy8Dbv8prpJ/0kKhlGeJY"
        "ozo2t60EG8L0561g13R29LvMR5hyvGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+"
        "vw1HocOAZtWK0z3r26uA8kQYOKX9Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQAp"
        "fc9jB9nTzphOgM4JiEYvlV8FLhg9yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68"
        "i6T4nNq7NWC+UNVjQHxNQMQMzU6lWCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoV"
        "PpY72+eVthKzpMeyHkBn7ciumk5qgLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUy"
        "wQIDAQAB"
        "-----END PUBLIC KEY-----";

    std::vector<uint8_t> iv = { 0x29, 0x43, 0x00, 0x4d, 0x3b, 0xb1, 0xc5, 0x7c, 0xff, 0xd8, 0x83, 0xc1, 0xe8, 0xd0, 0x75, 0xf3 };

    MIDEncryption::MIDEncryption(std::vector<uint8_t> mid) 
        :mid_(mid) {
    }

    MIDEncryption::~MIDEncryption() {

    }

    void StringToMemory(const std::string &inBuffer, std::vector<unsigned char> &outBuffer) {
        size_t bufferSize = inBuffer.size();
        outBuffer.resize(bufferSize / 2);

        unsigned char *pCurrent = &outBuffer[0];
        std::string::value_type byte;
        for (size_t index = 0; index < bufferSize; index += 2) {
            byte = inBuffer[index + 1];
            if (byte >= 'a' && byte <= 'f')
                *pCurrent = byte - 'a' + 10;
            else
                *pCurrent = byte - '0';
            byte = inBuffer[index];
            if (byte >= 'a' && byte <= 'f')
                *pCurrent = *pCurrent | (byte - 'a' + 10) << 4;
            else
                *pCurrent = *pCurrent | (byte - '0') << 4;
            pCurrent++;
        }
    }

    bool MIDEncryption::ParseData(std::string &encryptionData, const std::string &signature, std::vector<uint8_t> &decryptionData) {
        std::vector<uint8_t> encryptionDataBuffer;
        StringToMemory(encryptionData, encryptionDataBuffer);

        std::vector<uint8_t> signatureBuffer;
        StringToMemory(signature, signatureBuffer);

        if (!CryptoAPI::RSAEncryption::VerifySignature(encryptionDataBuffer, signatureBuffer, PUBLICKEY)) {
            return false;
        }

        CryptoAPI::AESEncryption::CBCDecrypt(encryptionDataBuffer, decryptionData, mid_, iv);
        return true;
    }
}