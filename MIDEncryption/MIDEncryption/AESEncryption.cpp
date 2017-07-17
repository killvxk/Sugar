#include "stdafx.h"
#include <Windows.h>
#include <wincrypt.h>
#include <algorithm>
#include <openssl/aes.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl\md5.h>
#include "AESEncryption.h"

namespace MIDEncryption
{
    void AESEncryption::Encrypt(std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
        AES_KEY aesKey;

        std::vector<uint8_t> keyBuffer = key;
        keyBuffer.resize(((key.size() + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE);

        {
            MD5_CTX md5_ctx;
            MD5_Init(&md5_ctx);
            MD5_Update(&md5_ctx, &key[0], key.size());
            MD5_Final(&keyBuffer[0], &md5_ctx);
        }

        {
            std::vector<uint8_t> buffer1(64, 0x36);
            for (int index = 0; index < keyBuffer.size(); ++index) {
                buffer1[index] ^= keyBuffer[index];
            }

            std::vector<uint8_t> buffer1Md5(keyBuffer.size());
            {
                MD5_CTX md5_ctx;
                MD5_Init(&md5_ctx);
                MD5_Update(&md5_ctx, &buffer1[0], buffer1.size());
                MD5_Final(&buffer1Md5[0], &md5_ctx);
            }

            for (int index = 0; index < keyBuffer.size(); ++index) {
                keyBuffer[index] = buffer1[index];
            }

            AES_set_encrypt_key(&buffer1Md5[0], 128, &aesKey);
        }

        sourceBuffer.resize(((sourceBuffer.size() + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE);
        destinationBuffer.resize(sourceBuffer.size());
        std::vector<uint8_t> ivBuffer = iv;
        AES_cbc_encrypt(&sourceBuffer[0], &destinationBuffer[0], sourceBuffer.size(), &aesKey, (unsigned char *)&ivBuffer[0], AES_ENCRYPT);
    }

    void AESEncryption::Decrypt(std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
        AES_KEY aesKey;

        std::vector<uint8_t> keyBuffer = key;
        keyBuffer.resize(((key.size() + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE);
        AES_set_decrypt_key(&keyBuffer[0], 128, &aesKey);

        destinationBuffer.resize(sourceBuffer.size());
        std::vector<uint8_t> ivBuffer = iv;
        AES_cbc_encrypt(&sourceBuffer[0], &destinationBuffer[0], sourceBuffer.size(), &aesKey, (unsigned char *)&iv[0], AES_DECRYPT);
    }

    #define AES128_BLOCK_SIZE 16
    void AESEncryption::CBCEncrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
        HCRYPTPROV hCryptProv = NULL;
        HCRYPTKEY hKey = 0;
        HCRYPTHASH hHash = 0;

        destinationBuffer = sourceBuffer;
        destinationBuffer.resize(((destinationBuffer.size() + AES128_BLOCK_SIZE - 1) / AES128_BLOCK_SIZE) * AES128_BLOCK_SIZE);

        if (!CryptAcquireContext(&hCryptProv,
            NULL,
            NULL, 
            PROV_RSA_AES,
            CRYPT_VERIFYCONTEXT))
        {
            DWORD dwLastErr = GetLastError();

            if (NTE_BAD_KEYSET == dwLastErr)
            {
                throw std::exception("CryptAcquireContext Failed");
            }
            else {
                if (!CryptAcquireContext(&hCryptProv,
                    NULL,
                    NULL,
                    PROV_RSA_AES,
                    CRYPT_NEWKEYSET))
                {
                    throw std::exception("CryptAcquireContext Failed");
                }
            }
        }

        if (!CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash)) {
            throw std::exception("CryptCreateHash Failed");
        }

        if (!CryptHashData(
            hHash,
            (BYTE *)&key[0],
            key.size(),
            0)) {
            throw std::exception("CryptHashData Failed");
        }

        if (!CryptDeriveKey(hCryptProv, CALG_AES_128, hHash, CRYPT_EXPORTABLE, &hKey)) {
            throw std::exception("CryptDeriveKey Failed");
        }

        CryptDestroyHash(hHash);
        CryptSetKeyParam(hKey, KP_IV, &iv[0], 0);
        
        int count = destinationBuffer.size() / AES128_BLOCK_SIZE;
        for (int index = 0; index < count; ++index) {
            DWORD dwNumberOfBytesRead = AES128_BLOCK_SIZE;
            CryptEncrypt(hKey, NULL, (index == (count - 1)), 0, &destinationBuffer[index * AES128_BLOCK_SIZE], &dwNumberOfBytesRead, 2 * AES128_BLOCK_SIZE);
        }
        
        CryptDestroyKey(hKey);
        CryptReleaseContext(hCryptProv, 0);
    }

    void AESEncryption::CBCDecrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
        HCRYPTPROV hCryptProv = NULL;
        HCRYPTKEY hKey = 0;
        HCRYPTHASH hHash = 0;

        destinationBuffer = sourceBuffer;

        if (!CryptAcquireContext(&hCryptProv,
            NULL,
            NULL,
            PROV_RSA_AES,
            CRYPT_VERIFYCONTEXT))
        {
            DWORD dwLastErr = GetLastError();

            if (NTE_BAD_KEYSET == dwLastErr)
            {
                throw std::exception("CryptAcquireContext Failed");
            }
            else {
                if (!CryptAcquireContext(&hCryptProv,
                    NULL,
                    NULL,
                    PROV_RSA_AES,
                    CRYPT_NEWKEYSET))
                {
                    throw std::exception("CryptAcquireContext Failed");
                }
            }
        }

        if (!CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash)) {
            throw std::exception("CryptCreateHash Failed");
        }

        if (!CryptHashData(
            hHash,
            (BYTE *)&key[0],
            key.size(),
            0)) {
            throw std::exception("CryptHashData Failed");
        }

        if (!CryptDeriveKey(hCryptProv, CALG_AES_128, hHash, CRYPT_EXPORTABLE, &hKey)) {
            throw std::exception("CryptDeriveKey Failed");
        }

        CryptDestroyHash(hHash);
        CryptSetKeyParam(hKey, KP_IV, &iv[0], 0);

        int count = destinationBuffer.size() / AES128_BLOCK_SIZE;
        for (int index = 0; index < count; ++index) {
            DWORD dwNumberOfBytesRead = AES128_BLOCK_SIZE;
            DWORD length = 0;
            CryptDecrypt(hKey, NULL, (index == (count - 1)), 0, &destinationBuffer[index * AES128_BLOCK_SIZE], &dwNumberOfBytesRead);
        }

        CryptDestroyKey(hKey);
        CryptReleaseContext(hCryptProv, 0);
    }
}