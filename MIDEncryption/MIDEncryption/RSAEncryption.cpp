#include "stdafx.h"
#include <exception>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <Windows.h>
#include <wincrypt.h>
#include "RSAEncryption.h"

namespace MIDEncryption
{
    void RSAEncryption::PublicEncrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &publicKey) {
        RSA *rsa = NULL;
        BIO *keybio;
        keybio = BIO_new_mem_buf(&publicKey[0], -1);
        if (keybio == NULL) {
            throw std::exception("Failed to create key BIO");
        }

        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
        destinationBuffer.resize(RSA_size(rsa));

        int result = -1;
        if ((result = RSA_public_encrypt(sourceBuffer.size(), &sourceBuffer[0], &destinationBuffer[0], rsa, RSA_PKCS1_PADDING)) != -1) {
            destinationBuffer.resize(result);
        }
    }

    void RSAEncryption::PrivateDecrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &privateKey) {
        RSA *rsa = NULL;
        BIO *keybio;
        keybio = BIO_new_mem_buf(&privateKey[0], -1);
        if (keybio == NULL) {
            throw std::exception("Failed to create key BIO");
        }

        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
        destinationBuffer.resize(RSA_size(rsa));

        int result = -1;
        if ((result = RSA_private_decrypt(sourceBuffer.size(), &sourceBuffer[0], &destinationBuffer[0], rsa, RSA_PKCS1_PADDING)) != -1) {
            destinationBuffer.resize(result);
        }
    }

    void RSAEncryption::PrivateEncrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &privateKey) {
        RSA *rsa = NULL;
        BIO *keybio;
        keybio = BIO_new_mem_buf(&privateKey[0], -1);
        if (keybio == NULL) {
            throw std::exception("Failed to create key BIO");
        }

        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
        destinationBuffer.resize(RSA_size(rsa));
        int result = -1;
        if ((result = RSA_private_encrypt(sourceBuffer.size(), &sourceBuffer[0], &destinationBuffer[0], rsa, RSA_PKCS1_PADDING)) != -1) {
            destinationBuffer.resize(result);
        }
    }

    void RSAEncryption::PublicDecrypt(const std::vector<uint8_t> &sourceBuffer, std::vector<uint8_t> &destinationBuffer, const std::vector<uint8_t> &publicKey) {
        RSA *rsa = NULL;
        BIO *keybio;
        keybio = BIO_new_mem_buf(&publicKey[0], -1);
        if (keybio == NULL) {
            throw std::exception("Failed to create key BIO");
        }

        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
        destinationBuffer.resize(RSA_size(rsa));

        int result = -1;
        if ((result = RSA_public_decrypt(sourceBuffer.size(), &sourceBuffer[0], &destinationBuffer[0], rsa, RSA_PKCS1_PADDING)) != -1) {
            destinationBuffer.resize(result);
        }
    }

    void RSAEncryption::Signature(const std::vector<uint8_t> &dataBuffer, std::vector<uint8_t> &signatureData, const std::string privateKey) {
        HCRYPTPROV hCryptProv = NULL;
        HCRYPTKEY hKey = 0;
        HCRYPTKEY hHash = 0;

        DWORD dwBufferLen = 0, cbKeyBlob = 0, cbSignature = 0;
        std::vector<uint8_t> buffer, keyBlob;

        if (!CryptStringToBinaryA(privateKey.c_str(), 0, CRYPT_STRING_BASE64HEADER, NULL, &dwBufferLen, NULL, NULL)) {
            throw std::exception("CryptStringToBinary Failed");
        }

        buffer.resize(dwBufferLen);
        if (!CryptStringToBinaryA(privateKey.c_str(), 0, CRYPT_STRING_BASE64HEADER, &buffer[0], &dwBufferLen, NULL, NULL)) {
            throw std::exception("CryptStringToBinary Failed");
        }

        if (!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, &buffer[0], dwBufferLen, 0, NULL, NULL, &cbKeyBlob)) {
            throw std::exception("CryptDecodeObjectEx Failed");
        }

        keyBlob.resize(cbKeyBlob);
        if (!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, &buffer[0], dwBufferLen, 0, NULL, &keyBlob[0], &cbKeyBlob)) {
            throw std::exception("CryptDecodeObjectEx Failed");
        }

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

        if (!CryptImportKey(hCryptProv, &keyBlob[0], cbKeyBlob, NULL, 0, &hKey)) {
            throw std::exception("CryptImportKey Failed");
        }

        if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
            throw std::exception("CryptCreateHash Failed");
        }

        if (!CryptHashData(
            hHash,
            (BYTE *)&dataBuffer[0],
            dataBuffer.size(),
            0)) {
            throw std::exception("CryptHashData Failed");
        }

        if (!CryptSignHash(hHash, AT_KEYEXCHANGE, NULL, 0, NULL, &cbSignature)) {
            throw std::exception("CryptSignHash Failed");
        }

        signatureData.resize(cbSignature);
        if (!CryptSignHash(hHash, AT_KEYEXCHANGE, NULL, 0, &signatureData[0], &cbSignature)) {
            throw std::exception("CryptSignHash Failed");
        }
    }

    bool RSAEncryption::VerifySignature(const std::vector<uint8_t> &dataBuffer, const std::vector<uint8_t> &signatureData, const std::string &publicKey) {
        HCRYPTPROV hCryptProv = NULL;
        HCRYPTKEY hKey = 0;
        HCRYPTKEY hHash = 0;

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

        DWORD dwBufferLen = 0, cbKeyBlob = 0;
        std::vector<uint8_t> buffer, keyBlob;

        if (!CryptStringToBinaryA(publicKey.c_str(), 0, CRYPT_STRING_BASE64HEADER, NULL, &dwBufferLen, NULL, NULL)) {
            throw std::exception("CryptStringToBinary Failed");
        }

        buffer.resize(dwBufferLen);
        if (!CryptStringToBinaryA(publicKey.c_str(), 0, CRYPT_STRING_BASE64HEADER, &buffer[0], &dwBufferLen, NULL, NULL)) {
            throw std::exception("CryptStringToBinary Failed");
        }

        if (!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, X509_PUBLIC_KEY_INFO, &buffer[0], dwBufferLen, 0, NULL, NULL, &cbKeyBlob)) {
            throw std::exception("CryptDecodeObjectEx Failed");
        }

        keyBlob.resize(cbKeyBlob);
        if (!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, X509_PUBLIC_KEY_INFO, &buffer[0], dwBufferLen, 0, NULL, &keyBlob[0], &cbKeyBlob)) {
            throw std::exception("CryptDecodeObjectEx Failed");
        }

        if (!CryptImportPublicKeyInfo(hCryptProv,
            X509_ASN_ENCODING,
            (PCERT_PUBLIC_KEY_INFO)&keyBlob[0], &hKey)) {
            throw std::exception("CryptImportPublicKeyInfo Failed");
        }

        if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
            throw std::exception("CryptCreateHash Failed");
        }

        if (!CryptHashData(
            hHash,
            (BYTE *)&dataBuffer[0],
            dataBuffer.size(),
            0)) {
            throw std::exception("CryptHashData Failed");
        }

        return CryptVerifySignature(hHash, &signatureData[0], signatureData.size(), hKey, NULL, 0) == TRUE;
    }
}