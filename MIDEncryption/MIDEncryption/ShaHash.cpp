#include "stdafx.h"
#include <Windows.h>
#include <wincrypt.h>
#include "ShaHash.h"

namespace MIDEncryption
{
    void Sha::Hash256(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer) {
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

        if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
            throw std::exception("CryptCreateHash Failed");
        }

        if (!CryptHashData(
            hHash,
            (BYTE *)&sourceBuffer[0],
            sourceBuffer.size(),
            0)) {
            throw std::exception("CryptHashData Failed");
        }

        destinationBuffer.resize(32);
        DWORD hash_len = 32;
        if (!CryptGetHashParam(hHash, HP_HASHVAL, (BYTE*)&destinationBuffer[0], &hash_len, 0)) {
            throw std::exception("CryptGetHashParam Failed");
        }
    }
}
