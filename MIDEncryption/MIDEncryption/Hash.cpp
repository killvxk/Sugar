#include "stdafx.h"
#include "Hash.h"

namespace MIDEncryption
{
    namespace CryptoAPI
    {
        void Hash::Sha256(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer) {
            doHash(CALG_SHA_256, sourceBuffer, destinationBuffer);
        }

        void Hash::MD5(std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer) {
            doHash(CALG_MD5, sourceBuffer, destinationBuffer);
        }

        void Hash::doHash(ALG_ID Algid, const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer) {
            HCRYPTPROV hCryptProv = NULL;
            HCRYPTHASH hHash = 0;

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

            if (!CryptCreateHash(hCryptProv, Algid, 0, 0, &hHash)) {
                throw std::exception("CryptCreateHash Failed");
            }

            if (!CryptHashData(
                hHash,
                (BYTE *)&sourceBuffer[0],
                sourceBuffer.size(),
                0)) {
                throw std::exception("CryptHashData Failed");
            }

            DWORD hash_len = 0;
            DWORD dwCount;
            if (!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&hash_len, &dwCount, 0)) {
                throw std::exception("CryptGetHashParam Failed");
            }

            destinationBuffer.resize(hash_len);
            if (!CryptGetHashParam(hHash, HP_HASHVAL, (BYTE*)&destinationBuffer[0], &hash_len, 0)) {
                throw std::exception("CryptGetHashParam Failed");
            }

            CryptDestroyHash(hHash);
            CryptReleaseContext(hCryptProv, 0);
        }
    }

    namespace NextCryptoAPI
    {
        void Hash::Sha256(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer) {
            doHash(BCRYPT_SHA256_ALGORITHM, sourceBuffer, destinationBuffer);
        }

        void Hash::MD5(std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer) {
            doHash(BCRYPT_MD5_ALGORITHM, sourceBuffer, destinationBuffer);
        }

        void Hash::doHash(LPCWSTR pszAlgId, const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer) {
            NTSTATUS                status = STATUS_UNSUCCESSFUL;

            BCRYPT_ALG_HANDLE       hAlg = NULL;
            // Open an algorithm handle.
            if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
                &hAlg,
                pszAlgId,
                NULL,
                0)))
            {
                throw std::exception("BCryptOpenAlgorithmProvider Failed");
            }

            BCRYPT_HASH_HANDLE      hHash = NULL;
            //create a hash
            if (!NT_SUCCESS(status = BCryptCreateHash(
                hAlg,
                &hHash,
                NULL,
                0,
                NULL,
                0,
                0)))
            {
                throw std::exception("BCryptCreateHash Failed");
            }

            //hash some data
            if (!NT_SUCCESS(status = BCryptHashData(
                hHash,
                (PBYTE)&sourceBuffer[0],
                sourceBuffer.size(),
                0)))
            {
                throw std::exception("BCryptHashData Failed");
            }

            DWORD hash_len = 0;
            DWORD cbData = 0;
            //calculate the length of the hash
            if (!NT_SUCCESS(status = BCryptGetProperty(
                hAlg,
                BCRYPT_HASH_LENGTH,
                (PBYTE)&hash_len,
                sizeof(DWORD),
                &cbData,
                0)))
            {
                wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
            }

            destinationBuffer.resize(hash_len);
            //close the hash
            if (!NT_SUCCESS(status = BCryptFinishHash(
                hHash,
                &destinationBuffer[0],
                hash_len,
                0)))
            {
                throw std::exception("BCryptFinishHash Failed");
            }

            if (hHash)
            {
                BCryptDestroyHash(hHash);
            }

            if (hAlg)
            {
                BCryptCloseAlgorithmProvider(hAlg, 0);
            }
        }
    }
}
