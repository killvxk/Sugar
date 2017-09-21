// TestConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../MIDEncryption/RSAEncryption.h"
#include "../MIDEncryption/AESEncryption.h"
#include "../MIDEncryption/MIDEncryption.h"
#include "../MIDEncryption/Hash.h"
#include <Windows.h>
#include <wincrypt.h>

#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)

#define STATUS_UNSUCCESSFUL         ((NTSTATUS)0xC0000001L)

using namespace std;

std::string PUBLICKEY = "-----BEGIN PUBLIC KEY-----\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy8Dbv8prpJ/0kKhlGeJY"
"ozo2t60EG8L0561g13R29LvMR5hyvGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+"
"vw1HocOAZtWK0z3r26uA8kQYOKX9Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQAp"
"fc9jB9nTzphOgM4JiEYvlV8FLhg9yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68"
"i6T4nNq7NWC+UNVjQHxNQMQMzU6lWCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoV"
"PpY72+eVthKzpMeyHkBn7ciumk5qgLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUy"
"wQIDAQAB"
"-----END PUBLIC KEY-----";

std::string PRIVATEKEY = "-----BEGIN RSA PRIVATE KEY-----"
"MIIEowIBAAKCAQEAy8Dbv8prpJ/0kKhlGeJYozo2t60EG8L0561g13R29LvMR5hy"
"vGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+vw1HocOAZtWK0z3r26uA8kQYOKX9"
"Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQApfc9jB9nTzphOgM4JiEYvlV8FLhg9"
"yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68i6T4nNq7NWC+UNVjQHxNQMQMzU6l"
"WCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoVPpY72+eVthKzpMeyHkBn7ciumk5q"
"gLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUywQIDAQABAoIBADhg1u1Mv1hAAlX8"
"omz1Gn2f4AAW2aos2cM5UDCNw1SYmj+9SRIkaxjRsE/C4o9sw1oxrg1/z6kajV0e"
"N/t008FdlVKHXAIYWF93JMoVvIpMmT8jft6AN/y3NMpivgt2inmmEJZYNioFJKZG"
"X+/vKYvsVISZm2fw8NfnKvAQK55yu+GRWBZGOeS9K+LbYvOwcrjKhHz66m4bedKd"
"gVAix6NE5iwmjNXktSQlJMCjbtdNXg/xo1/G4kG2p/MO1HLcKfe1N5FgBiXj3Qjl"
"vgvjJZkh1as2KTgaPOBqZaP03738VnYg23ISyvfT/teArVGtxrmFP7939EvJFKpF"
"1wTxuDkCgYEA7t0DR37zt+dEJy+5vm7zSmN97VenwQJFWMiulkHGa0yU3lLasxxu"
"m0oUtndIjenIvSx6t3Y+agK2F3EPbb0AZ5wZ1p1IXs4vktgeQwSSBdqcM8LZFDvZ"
"uPboQnJoRdIkd62XnP5ekIEIBAfOp8v2wFpSfE7nNH2u4CpAXNSF9HsCgYEA2l8D"
"JrDE5m9Kkn+J4l+AdGfeBL1igPF3DnuPoV67BpgiaAgI4h25UJzXiDKKoa706S0D"
"4XB74zOLX11MaGPMIdhlG+SgeQfNoC5lE4ZWXNyESJH1SVgRGT9nBC2vtL6bxCVV"
"WBkTeC5D6c/QXcai6yw6OYyNNdp0uznKURe1xvMCgYBVYYcEjWqMuAvyferFGV+5"
"nWqr5gM+yJMFM2bEqupD/HHSLoeiMm2O8KIKvwSeRYzNohKTdZ7FwgZYxr8fGMoG"
"PxQ1VK9DxCvZL4tRpVaU5Rmknud9hg9DQG6xIbgIDR+f79sb8QjYWmcFGc1SyWOA"
"SkjlykZ2yt4xnqi3BfiD9QKBgGqLgRYXmXp1QoVIBRaWUi55nzHg1XbkWZqPXvz1"
"I3uMLv1jLjJlHk3euKqTPmC05HoApKwSHeA0/gOBmg404xyAYJTDcCidTg6hlF96"
"ZBja3xApZuxqM62F6dV4FQqzFX0WWhWp5n301N33r0qR6FumMKJzmVJ1TA8tmzEF"
"yINRAoGBAJqioYs8rK6eXzA8ywYLjqTLu/yQSLBn/4ta36K8DyCoLNlNxSuox+A5"
"w6z2vEfRVQDq4Hm4vBzjdi3QfYLNkTiTqLcvgWZ+eX44ogXtdTDO7c+GeMKWz4XX"
"uJSUVL5+CVjKLjZEJ6Qc2WZLl94xSwL71E41H4YciVnSCQxVc4Jw"
"-----END RSA PRIVATE KEY-----";

std::vector<BYTE> MakePatternBytes(size_t a_Length)
{
    std::vector<BYTE> result(a_Length);
    for (size_t i = 0; i < result.size(); i++)
    {
        result[i] = (BYTE)i;
    }

    return result;
}

std::vector<BYTE> MakeRandomBytes(size_t a_Length)
{
    std::vector<BYTE> result(a_Length);
    for (size_t i = 0; i < result.size(); i++)
    {
        result[i] = (BYTE)rand();
    }

    return result;
}
#define DATA_TO_ENCRYPT  "Test Data"


const BYTE rgbPlaintext[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

static const BYTE rgbIV[] =
{
    0x29, 0x43, 0x00, 0x4d, 0x3b, 0xb1, 0xc5, 0x7c, 0xff, 0xd8, 0x83, 0xc1, 0xe8, 0xd0, 0x75, 0xf3
};

static const BYTE rgbAES128Key[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

void PrintBytes(
    IN BYTE     *pbPrintData,
    IN DWORD    cbDataLen)
{
    DWORD dwCount = 0;

    for (dwCount = 0; dwCount < cbDataLen; dwCount++)
    {
        printf("0x%02x, ", pbPrintData[dwCount]);

        if (0 == (dwCount + 1) % 10) putchar('\n');
    }

}


void __cdecl wmain(
    int                      argc,
    __in_ecount(argc) LPWSTR *wargv)
{

    BCRYPT_ALG_HANDLE       hAesAlg = NULL;
    BCRYPT_KEY_HANDLE       hKey = NULL;
    NTSTATUS                status = STATUS_UNSUCCESSFUL;
    DWORD                   cbCipherText = 0,
        cbPlainText = 0,
        cbData = 0,
        cbKeyObject = 0,
        cbBlockLen = 0,
        cbBlob = 0;
    PBYTE                   pbCipherText = NULL,
        pbPlainText = NULL,
        pbKeyObject = NULL,
        pbIV = NULL,
        pbBlob = NULL;
    std::vector<uint8_t> mid = { '1', '1','1', '1', '1', '1' }, nextCryptoAPIHashMD5;
    std::string source = "\"11111111111111\"\"11111111111111\"";
    std::vector<uint8_t> sourceBuffer(source.size());
    memcpy(&sourceBuffer[0], &source[0], source.size());

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(wargv);


    // Open an algorithm handle.
    if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
        &hAesAlg,
        BCRYPT_AES_ALGORITHM,
        NULL,
        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
    }

    // Calculate the size of the buffer to hold the KeyObject.
    if (!NT_SUCCESS(status = BCryptGetProperty(
        hAesAlg,
        BCRYPT_OBJECT_LENGTH,
        (PBYTE)&cbKeyObject,
        sizeof(DWORD),
        &cbData,
        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
    }

    // Allocate the key object on the heap.
    pbKeyObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbKeyObject);
    if (NULL == pbKeyObject)
    {
        wprintf(L"**** memory allocation failed\n");
    }

    // Calculate the block length for the IV.
    if (!NT_SUCCESS(status = BCryptGetProperty(
        hAesAlg,
        BCRYPT_BLOCK_LENGTH,
        (PBYTE)&cbBlockLen,
        sizeof(DWORD),
        &cbData,
        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
    }

    // Determine whether the cbBlockLen is not longer than the IV length.
    if (cbBlockLen > sizeof(rgbIV))
    {
        wprintf(L"**** block length is longer than the provided IV length\n");
    }

    // Allocate a buffer for the IV. The buffer is consumed during the 
    // encrypt/decrypt process.
    pbIV = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbBlockLen);
    if (NULL == pbIV)
    {
        wprintf(L"**** memory allocation failed\n");
    }

    memcpy(pbIV, rgbIV, cbBlockLen);

    if (!NT_SUCCESS(status = BCryptSetProperty(
        hAesAlg,
        BCRYPT_CHAINING_MODE,
        (PBYTE)BCRYPT_CHAIN_MODE_CBC,
        sizeof(BCRYPT_CHAIN_MODE_CBC),
        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptSetProperty\n", status);
    }

    MIDEncryption::NextCryptoAPI::Hash::MD5(mid, nextCryptoAPIHashMD5);

    // Generate the key from supplied input key bytes.
    if (!NT_SUCCESS(status = BCryptGenerateSymmetricKey(
        hAesAlg,
        &hKey,
        pbKeyObject,
        cbKeyObject,
        (PBYTE)&nextCryptoAPIHashMD5[0],
        nextCryptoAPIHashMD5.size(),
        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptGenerateSymmetricKey\n", status);
    }


    // Save another copy of the key for later.
    if (!NT_SUCCESS(status = BCryptExportKey(
        hKey,
        NULL,
        BCRYPT_OPAQUE_KEY_BLOB,
        NULL,
        0,
        &cbBlob,
        0)))
    {

    }


    // Allocate the buffer to hold the BLOB.
    pbBlob = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbBlob);
    if (NULL == pbBlob)
    {
        wprintf(L"**** memory allocation failed\n");
    }

    if (!NT_SUCCESS(status = BCryptExportKey(
        hKey,
        NULL,
        BCRYPT_OPAQUE_KEY_BLOB,
        pbBlob,
        cbBlob,
        &cbBlob,
        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptExportKey\n", status);
    }

    cbPlainText = sizeof(rgbPlaintext);
    pbPlainText = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbPlainText);
    if (NULL == pbPlainText)
    {
        wprintf(L"**** memory allocation failed\n");
    }

    memcpy(pbPlainText, rgbPlaintext, sizeof(rgbPlaintext));

    //
    // Get the output buffer size.
    //
    if (!NT_SUCCESS(status = BCryptEncrypt(
        hKey,
        &sourceBuffer[0],
        sourceBuffer.size(),
        NULL,
        pbIV,
        cbBlockLen,
        NULL,
        0,
        &cbCipherText,
        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptEncrypt\n", status);
    }

    pbCipherText = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbCipherText);
    if (NULL == pbCipherText)
    {
        wprintf(L"**** memory allocation failed\n");
    }

    // Use the key to encrypt the plaintext buffer.
    // For block sized messages, block padding will add an extra block.
    if (!NT_SUCCESS(status = BCryptEncrypt(
        hKey,
        &sourceBuffer[0],
        sourceBuffer.size(),
        NULL,
        pbIV,
        cbBlockLen,
        pbCipherText,
        cbCipherText,
        &cbData,
        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptEncrypt\n", status);
    }

    // Destroy the key and reimport from saved BLOB.
    if (!NT_SUCCESS(status = BCryptDestroyKey(hKey)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptDestroyKey\n", status);
    }
    hKey = 0;

    std::vector<uint8_t> iv = { 0x29, 0x43, 0x00, 0x4d, 0x3b, 0xb1, 0xc5, 0x7c, 0xff, 0xd8, 0x83, 0xc1, 0xe8, 0xd0, 0x75, 0xf3 };
    std::vector<uint8_t> cryptoAPICBCEncrypt, cryptoAPICBCDescrypt, nextCryptoAPICBCEncrypt, nextCryptoAPICBCDescrypt, opensslCBCEncrypt;
    MIDEncryption::OpenSSL::AESEncryption::Encrypt(sourceBuffer, opensslCBCEncrypt, mid, iv);
    MIDEncryption::CryptoAPI::AESEncryption::CBCEncrypt(sourceBuffer, cryptoAPICBCEncrypt, mid, iv);
    MIDEncryption::NextCryptoAPI::AESEncryption::CBCEncrypt(sourceBuffer, nextCryptoAPICBCEncrypt, mid, iv);
    MIDEncryption::CryptoAPI::AESEncryption::CBCDecrypt(cryptoAPICBCEncrypt, sourceBuffer, mid, iv);
}
//void main(void)
//{
//    std::vector<uint8_t> mid = { '1', '1','1', '1', '1', '1' };
//    std::vector<uint8_t> cryptoAPIHashSha256, nextCryptoAPIHashSha256, cryptoAPIHashMD5, nextCryptoAPIHashMD5;
//    MIDEncryption::CryptoAPI::Hash::Sha256(mid, cryptoAPIHashSha256);
//    MIDEncryption::NextCryptoAPI::Hash::Sha256(mid, nextCryptoAPIHashSha256);
//    MIDEncryption::CryptoAPI::Hash::MD5(mid, cryptoAPIHashMD5);
//    MIDEncryption::NextCryptoAPI::Hash::MD5(mid, nextCryptoAPIHashMD5);
//    //std::vector<uint8_t> publicKeyBuffer(PUBLICKEY.size());
//    //memcpy(&publicKeyBuffer[0], &PUBLICKEY[0], PUBLICKEY.size());
//    //std::vector<uint8_t> privateKeyBuffer(PRIVATEKEY.size());
//    //memcpy(&privateKeyBuffer[0], &PRIVATEKEY[0], PRIVATEKEY.size());
//    {
//        std::string source = "\"11111111111111\"";
//        std::vector<uint8_t> sourceBuffer(source.size());
//        memcpy(&sourceBuffer[0], &source[0], source.size());
//        std::vector<uint8_t> cryptoAPICBCEncrypt, cryptoAPICBCDescrypt, nextCryptoAPICBCEncrypt, nextCryptoAPICBCDescrypt;
//        //MIDEncryption::RSAEncryption::PrivateEncrypt(sourceBuffer, destinationBuffer, privateKeyBuffer);
//        //MIDEncryption::RSAEncryption::PublicDecrypt(destinationBuffer, sourceBuffer, publicKeyBuffer);
//        //std::vector<uint8_t> content = { 0x95, 0x14, 0x24, 0x29, 0x83, 0x19, 0x35, 0xda, 0xbb, 0x3a, 0x17, 0x2a, 0xa9, 0xbc, 0xa6, 0x2a, 0x80, 0x5e, 0x4d, 0x60, 0x48, 0x32, 0xbd, 0x70, 0xef, 0xec, 0x7c, 0x91, 0xc9, 0x5b, 0x38, 0x22 };
//        //std::vector<uint8_t> signature;
//        //MIDEncryption::RSAEncryption::Signature(content, signature, PRIVATEKEY);
//
//        std::vector<uint8_t> iv = { 0x29, 0x43, 0x00, 0x4d, 0x3b, 0xb1, 0xc5, 0x7c, 0xff, 0xd8, 0x83, 0xc1, 0xe8, 0xd0, 0x75, 0xf3 };
//        //MIDEncryption::OpenSSL::AESEncryption::Encrypt(sourceBuffer, destinationBuffer, mid, iv);
//        MIDEncryption::CryptoAPI::AESEncryption::CBCEncrypt(sourceBuffer, cryptoAPICBCEncrypt, mid, iv);
//        MIDEncryption::NextCryptoAPI::AESEncryption::CBCEncrypt(sourceBuffer, nextCryptoAPICBCEncrypt, mid, iv);
//        MIDEncryption::CryptoAPI::AESEncryption::CBCDecrypt(cryptoAPICBCEncrypt, sourceBuffer, mid, iv);
//    }
//
//    std::string source = "823ce34c65b99abbf371f6593336693f0d89bd9047fe34da34103aef11e24dcfd5937514df7db92aa9a85e967d34e5d834707eca475d6a8e0406f5bbdbf8b1eb58291ac6932966a8ca73a19f5c8e1487b9d974aa44de86b9add01ebf023e28c7c6ccfd140456e633d8c3386465ae6acf1d285a9459c071a5b167e8c6b9342227370a149a97c5f43bbef3413102ad97ed6500251a24cb02724f171151ad4b980640dde3a28d00ddb13b1a8880e22e32e0b12fde8f4393580734a79165d138e4332a7464991f0da8971015ebeba384a5e45b3927947b53968b0bc265f02fa2670dccc56d1f613cf84cbc9a5350ceea5b2e9094449e0937fa905bd01c9096eba512f0bc82bed24cb8113297260d2b6d0efa15e303f10acd18f2fdfdfe3785a076b2a7085a9bbd21d9de80f64804810bdb9a97e3b3f4b3ed942a5e859c616a5d0fe279b6d1952a2ac1c4e08d290c7e0f9ea26eb1a17e07d80b52979c3aab0cc1efa8ab37fd80a261b2852b6db610e0a7c3e7";
//    std::string sign = "d13fdb8e3467b19f4af734621ebf2255437e0ed29b971904108e6cf4aae155854689bf886151e4ce419ef7d58eaca015c9ff110874591b3f8014b429ad132e68d7f836f10f8438fc8b7b5fb4a2ef5174c54d16d4c043135f7059942e310c993dbaca1a9f815c63b674d0a19f97256bfbebaa813ed21fa5a97bf8bf0bb476de4ef1fa005486825847b1774a7d4ee84e2b630f071facf3d75361c2573a30e3fb7351aa5d485fd1de681c2b7b4080a04f232b49a24f6897d8a4e0038465caf43ba0e8360ede0216b8c1fb1739421344e4e4935b916e94010e772087c7696d148d18af4e37e3ad99a009545d5793a4f5d46de3f65e93e4f88ae48549aa47cf9a46af";
//    MIDEncryption::MIDEncryption midEncryption(mid);
//    //midEncryption.ParseData(source, sign, destinationBuffer);
//}