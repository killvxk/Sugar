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
#include "SymmetricEncryption.h"
#include "Hash.h"

namespace MIDEncryption
{
    namespace OpenSSL
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

            //参见 http://msdn.microsoft.com/en-us/library/aa379916(v=vs.85).aspx remarks步骤  
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
    }

    #define BLOCK_SIZE_128 16
    #define BLOCK_SIZE_256 32
    namespace CryptoAPI
    {
        template <ALG_ID Algid = CALG_AES_128, int BlockSize = BLOCK_SIZE_128, ALG_ID Hashid = CALG_MD5>
        void Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
            HCRYPTPROV hCryptProv = NULL;
            HCRYPTKEY hKey = 0;
            HCRYPTHASH hHash = 0;

            destinationBuffer = sourceBuffer;
            destinationBuffer.resize(((destinationBuffer.size() + BlockSize - 1) / BlockSize) * BlockSize + BlockSize);

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

            if (!CryptCreateHash(hCryptProv, Hashid, 0, 0, &hHash)) {
                throw std::exception("CryptCreateHash Failed");
            }

            if (!CryptHashData(
                hHash,
                (BYTE *)&key[0],
                key.size(),
                0)) {
                throw std::exception("CryptHashData Failed");
            }

            if (!CryptDeriveKey(hCryptProv, Algid, hHash, CRYPT_EXPORTABLE, &hKey)) {
                throw std::exception("CryptDeriveKey Failed");
            }

            DWORD keyBlobLength = 45;
            std::vector<uint8_t> keyBlob(keyBlobLength);
            CryptExportKey(hKey, NULL, PLAINTEXTKEYBLOB, 0, &keyBlob[0], &keyBlobLength);

            CryptDestroyHash(hHash);
            CryptSetKeyParam(hKey, KP_IV, &iv[0], 0);

            DWORD dwNumberOfBytesRead = sourceBuffer.size();
            CryptEncrypt(hKey, NULL, TRUE, 0, &destinationBuffer[0], &dwNumberOfBytesRead, destinationBuffer.size());
            destinationBuffer.resize(dwNumberOfBytesRead);

            CryptDestroyKey(hKey);
            CryptReleaseContext(hCryptProv, 0);
        }

        template <ALG_ID Algid = CALG_AES_128, int BlockSize = BLOCK_SIZE_128, ALG_ID Hashid = CALG_MD5>
        void Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
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

            if (!CryptCreateHash(hCryptProv, Hashid, 0, 0, &hHash)) {
                throw std::exception("CryptCreateHash Failed");
            }

            if (!CryptHashData(
                hHash,
                (BYTE *)&key[0],
                key.size(),
                0)) {
                throw std::exception("CryptHashData Failed");
            }

            if (!CryptDeriveKey(hCryptProv, Algid, hHash, CRYPT_EXPORTABLE, &hKey)) {
                throw std::exception("CryptDeriveKey Failed");
            }

            CryptDestroyHash(hHash);
            CryptSetKeyParam(hKey, KP_IV, &iv[0], 0);

            DWORD dwNumberOfBytesRead = destinationBuffer.size();
            CryptDecrypt(hKey, NULL, TRUE, 0, &destinationBuffer[0], &dwNumberOfBytesRead);
            destinationBuffer.resize(dwNumberOfBytesRead);

            CryptDestroyKey(hKey);
            CryptReleaseContext(hCryptProv, 0);
        }

        void AESEncryption::CBC128Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
            CryptoAPI::Encrypt<CALG_AES_128>(sourceBuffer, destinationBuffer, key, iv);
        }

        void AESEncryption::CBC128Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
            CryptoAPI::Decrypt<CALG_AES_128>(sourceBuffer, destinationBuffer, key, iv);
        }

        void AESEncryption::CBC256Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
            CryptoAPI::Encrypt<CALG_AES_256, BLOCK_SIZE_256, CALG_SHA1>(sourceBuffer, destinationBuffer, key, iv);
        }

        void AESEncryption::CBC256Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
            CryptoAPI::Decrypt<CALG_AES_256, BLOCK_SIZE_256, CALG_SHA1>(sourceBuffer, destinationBuffer, key, iv);
        }

        void RC4Encryption::Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
            CryptoAPI::Encrypt<CALG_RC4>(sourceBuffer, destinationBuffer, key, iv);
        }

        void RC4Encryption::Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
            CryptoAPI::Decrypt<CALG_RC4>(sourceBuffer, destinationBuffer, key, iv);
        }
    }

    namespace NextCryptoAPI
    {
        void AESEncryption::CBC128Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
            NTSTATUS                status = STATUS_UNSUCCESSFUL;
            BCRYPT_ALG_HANDLE       hAesAlg = NULL;

            destinationBuffer = sourceBuffer;
            destinationBuffer.resize(((destinationBuffer.size() + BLOCK_SIZE_128 - 1) / BLOCK_SIZE_128) * BLOCK_SIZE_128 + BLOCK_SIZE_128);

            // Open an algorithm handle.
            if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
                &hAesAlg,
                BCRYPT_AES_ALGORITHM,
                NULL,
                0)))
            {
                throw std::exception("BCryptOpenAlgorithmProvider Failed");
            }

            std::vector<uint8_t> keyBuffer;
            keyBuffer.resize(BLOCK_SIZE_128);
            Key::CreateKey<Hash::MD5, 16>(key, keyBuffer);

            BCRYPT_KEY_HANDLE       hKey = NULL;
            // Generate the key from supplied input key bytes.
            if (!NT_SUCCESS(status = BCryptGenerateSymmetricKey(
                hAesAlg,
                &hKey,
                NULL,
                0,
                (PBYTE)&keyBuffer[0],
                keyBuffer.size(),
                0)))
            {
                throw std::exception("BCryptGenerateSymmetricKey Failed");
            }

            std::vector<uint8_t> ivBuffer = iv;
            DWORD dwNumberOfBytesRead = sourceBuffer.size();
            BCryptEncrypt(hKey, (PUCHAR)&sourceBuffer[0], sourceBuffer.size(), NULL, &ivBuffer[0], ivBuffer.size(), &destinationBuffer[0], destinationBuffer.size(), &dwNumberOfBytesRead, TRUE);
            destinationBuffer.resize(dwNumberOfBytesRead);

            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(hAesAlg, 0);
        }

        void AESEncryption::CBC128Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
            NTSTATUS                status = STATUS_UNSUCCESSFUL;
            BCRYPT_ALG_HANDLE       hAesAlg = NULL;

            destinationBuffer = sourceBuffer;

            // Open an algorithm handle.
            if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
                &hAesAlg,
                BCRYPT_AES_ALGORITHM,
                NULL,
                0)))
            {
                throw std::exception("BCryptOpenAlgorithmProvider Failed");
            }

            std::vector<uint8_t> keyBuffer;
            keyBuffer.resize(BLOCK_SIZE_128);
            Key::CreateKey<Hash::MD5, 16>(key, keyBuffer);

            BCRYPT_KEY_HANDLE       hKey = NULL;
            // Generate the key from supplied input key bytes.
            if (!NT_SUCCESS(status = BCryptGenerateSymmetricKey(
                hAesAlg,
                &hKey,
                NULL,
                0,
                (PBYTE)&keyBuffer[0],
                keyBuffer.size(),
                0)))
            {
                throw std::exception("BCryptGenerateSymmetricKey Failed");
            }

            std::vector<uint8_t> ivBuffer = iv;
            DWORD dwNumberOfBytesRead = sourceBuffer.size();
            BCryptDecrypt(hKey, (PUCHAR)&sourceBuffer[0], sourceBuffer.size(), NULL, &ivBuffer[0], ivBuffer.size(), &destinationBuffer[0], destinationBuffer.size(), &dwNumberOfBytesRead, TRUE);
            destinationBuffer.resize(dwNumberOfBytesRead);

            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(hAesAlg, 0);
        }

        void AESEncryption::CBC256Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
            NTSTATUS                status = STATUS_UNSUCCESSFUL;
            BCRYPT_ALG_HANDLE       hAesAlg = NULL;

            destinationBuffer = sourceBuffer;
            destinationBuffer.resize(((destinationBuffer.size() + BLOCK_SIZE_256 - 1) / BLOCK_SIZE_256) * BLOCK_SIZE_256);

            // Open an algorithm handle.
            if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
                &hAesAlg,
                BCRYPT_AES_ALGORITHM,
                NULL,
                0)))
            {
                throw std::exception("BCryptOpenAlgorithmProvider Failed");
            }

            std::vector<uint8_t> keyBuffer;
            keyBuffer.resize(BLOCK_SIZE_256);
            Key::CreateKey<Hash::Sha1, 20>(key, keyBuffer);

            BCRYPT_KEY_HANDLE       hKey = NULL;
            // Generate the key from supplied input key bytes.
            if (!NT_SUCCESS(status = BCryptGenerateSymmetricKey(
                hAesAlg,
                &hKey,
                NULL,
                0,
                (PBYTE)&keyBuffer[0],
                BLOCK_SIZE_256,
                0)))
            {
                throw std::exception("BCryptGenerateSymmetricKey Failed");
            }

            std::vector<uint8_t> ivBuffer = iv;
            DWORD dwNumberOfBytesRead = sourceBuffer.size();
            BCryptEncrypt(hKey, (PUCHAR)&sourceBuffer[0], sourceBuffer.size(), NULL, &ivBuffer[0], ivBuffer.size(), &destinationBuffer[0], destinationBuffer.size(), &dwNumberOfBytesRead, TRUE);

            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(hAesAlg, 0);
        }

        void AESEncryption::CBC256Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key, const std::vector<uint8_t> &iv) {
            NTSTATUS                status = STATUS_UNSUCCESSFUL;
            BCRYPT_ALG_HANDLE       hAesAlg = NULL;

            destinationBuffer = sourceBuffer;

            // Open an algorithm handle.
            if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
                &hAesAlg,
                BCRYPT_AES_ALGORITHM,
                NULL,
                0)))
            {
                throw std::exception("BCryptOpenAlgorithmProvider Failed");
            }

            std::vector<uint8_t> keyBuffer;
            keyBuffer.resize(BLOCK_SIZE_256);
            Key::CreateKey<Hash::Sha1, 20>(key, keyBuffer);

            BCRYPT_KEY_HANDLE       hKey = NULL;
            // Generate the key from supplied input key bytes.
            if (!NT_SUCCESS(status = BCryptGenerateSymmetricKey(
                hAesAlg,
                &hKey,
                NULL,
                0,
                (PBYTE)&keyBuffer[0],
                BLOCK_SIZE_256,
                0)))
            {
                throw std::exception("BCryptGenerateSymmetricKey Failed");
            }

            std::vector<uint8_t> ivBuffer = iv;
            DWORD dwNumberOfBytesRead = sourceBuffer.size();
            BCryptDecrypt(hKey, (PUCHAR)&sourceBuffer[0], sourceBuffer.size(), NULL, &ivBuffer[0], ivBuffer.size(), &destinationBuffer[0], destinationBuffer.size(), &dwNumberOfBytesRead, TRUE);
            destinationBuffer.resize(dwNumberOfBytesRead);

            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(hAesAlg, 0);
        }

        void RC4Encryption::Encrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key) {
            NTSTATUS                status = STATUS_UNSUCCESSFUL;
            BCRYPT_ALG_HANDLE       hAesAlg = NULL;

            destinationBuffer = sourceBuffer;
            destinationBuffer.resize(((destinationBuffer.size() + BLOCK_SIZE_128 - 1) / BLOCK_SIZE_128) * BLOCK_SIZE_128);

            // Open an algorithm handle.
            if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
                &hAesAlg,
                BCRYPT_RC4_ALGORITHM,
                NULL,
                0)))
            {
                throw std::exception("BCryptOpenAlgorithmProvider Failed");
            }

            std::vector<uint8_t> keyBuffer;
            Hash::MD5(key, keyBuffer);

            BCRYPT_KEY_HANDLE       hKey = NULL;
            // Generate the key from supplied input key bytes.
            if (!NT_SUCCESS(status = BCryptGenerateSymmetricKey(
                hAesAlg,
                &hKey,
                NULL,
                0,
                (PBYTE)&keyBuffer[0],
                keyBuffer.size(),
                0)))
            {
                throw std::exception("BCryptGenerateSymmetricKey Failed");
            }

            DWORD dwNumberOfBytesRead = sourceBuffer.size();
            BCryptEncrypt(hKey, (PUCHAR)&sourceBuffer[0], sourceBuffer.size(), NULL, NULL, 0, &destinationBuffer[0], destinationBuffer.size(), &dwNumberOfBytesRead, TRUE);
            destinationBuffer.resize(dwNumberOfBytesRead);

            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(hAesAlg, 0);
        }

        void RC4Encryption::Decrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &key) {
            NTSTATUS                status = STATUS_UNSUCCESSFUL;
            BCRYPT_ALG_HANDLE       hAesAlg = NULL;

            destinationBuffer = sourceBuffer;

            // Open an algorithm handle.
            if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
                &hAesAlg,
                BCRYPT_RC4_ALGORITHM,
                NULL,
                0)))
            {
                throw std::exception("BCryptOpenAlgorithmProvider Failed");
            }

            std::vector<uint8_t> keyBuffer;
            Hash::MD5(key, keyBuffer);

            BCRYPT_KEY_HANDLE       hKey = NULL;
            // Generate the key from supplied input key bytes.
            if (!NT_SUCCESS(status = BCryptGenerateSymmetricKey(
                hAesAlg,
                &hKey,
                NULL,
                0,
                (PBYTE)&keyBuffer[0],
                keyBuffer.size(),
                0)))
            {
                throw std::exception("BCryptGenerateSymmetricKey Failed");
            }

            DWORD dwNumberOfBytesRead = sourceBuffer.size();
            BCryptDecrypt(hKey, (PUCHAR)&sourceBuffer[0], sourceBuffer.size(), NULL, NULL, 0, &destinationBuffer[0], destinationBuffer.size(), &dwNumberOfBytesRead, TRUE);
            destinationBuffer.resize(dwNumberOfBytesRead);

            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(hAesAlg, 0);
        }
    }
}