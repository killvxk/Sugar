// TestConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../MIDEncryption/RSAEncryption.h"
#include "../MIDEncryption/SymmetricEncryption.h"
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

void main(void)
{
    std::vector<uint8_t> mid = { '1', '1','1', '1', '1', '1' };
    std::vector<uint8_t> cryptoAPIHashSha1, nextCryptoAPIHashSha1, cryptoAPIHashSha256, nextCryptoAPIHashSha256, cryptoAPIHashSha512, nextCryptoAPIHashSha512, cryptoAPIHashMD5, nextCryptoAPIHashMD5;
    MIDEncryption::CryptoAPI::Hash::Sha1(mid, cryptoAPIHashSha1);
    MIDEncryption::NextCryptoAPI::Hash::Sha1(mid, nextCryptoAPIHashSha1);
    MIDEncryption::CryptoAPI::Hash::Sha256(mid, cryptoAPIHashSha256);
    MIDEncryption::NextCryptoAPI::Hash::Sha256(mid, nextCryptoAPIHashSha256);
    MIDEncryption::CryptoAPI::Hash::Sha512(mid, cryptoAPIHashSha512);
    MIDEncryption::NextCryptoAPI::Hash::Sha512(mid, nextCryptoAPIHashSha512);
    MIDEncryption::CryptoAPI::Hash::MD5(mid, cryptoAPIHashMD5);
    MIDEncryption::NextCryptoAPI::Hash::MD5(mid, nextCryptoAPIHashMD5);
    {
        std::string source = "\"11111111111111\"";
        std::vector<uint8_t> sourceBuffer(source.size());
        memcpy(&sourceBuffer[0], &source[0], source.size());
        std::vector<uint8_t> cryptoAPICBCEncrypt, cryptoAPICBCDescrypt, nextCryptoAPICBCEncrypt, nextCryptoAPICBCDescrypt;

        std::vector<uint8_t> iv = { 0x29, 0x43, 0x00, 0x4d, 0x3b, 0xb1, 0xc5, 0x7c, 0xff, 0xd8, 0x83, 0xc1, 0xe8, 0xd0, 0x75, 0xf3, 0x29, 0x43, 0x00, 0x4d, 0x3b, 0xb1, 0xc5, 0x7c, 0xff, 0xd8, 0x83, 0xc1, 0xe8, 0xd0, 0x75, 0xf3 };
        MIDEncryption::CryptoAPI::AESEncryption::CBC256Encrypt(sourceBuffer, cryptoAPICBCEncrypt, mid, iv);
        MIDEncryption::NextCryptoAPI::AESEncryption::CBC256Encrypt(sourceBuffer, nextCryptoAPICBCEncrypt, mid, iv);
        MIDEncryption::CryptoAPI::AESEncryption::CBC128Decrypt(cryptoAPICBCEncrypt, sourceBuffer, mid, iv);
        //MIDEncryption::NextCryptoAPI::AESEncryption::CBCDecrypt(nextCryptoAPICBCEncrypt, sourceBuffer, mid, iv);

        //MIDEncryption::CryptoAPI::RC4Encryption::Encrypt(sourceBuffer, cryptoAPICBCEncrypt, mid, iv);
        //MIDEncryption::NextCryptoAPI::RC4Encryption::Encrypt(sourceBuffer, nextCryptoAPICBCEncrypt, mid);
        //MIDEncryption::CryptoAPI::RC4Encryption::Decrypt(cryptoAPICBCEncrypt, sourceBuffer, mid, iv);
        //MIDEncryption::NextCryptoAPI::RC4Encryption::Decrypt(nextCryptoAPICBCEncrypt, sourceBuffer, mid);
    }

    std::string source = "823ce34c65b99abbf371f6593336693f0d89bd9047fe34da34103aef11e24dcfd5937514df7db92aa9a85e967d34e5d834707eca475d6a8e0406f5bbdbf8b1eb58291ac6932966a8ca73a19f5c8e1487b9d974aa44de86b9add01ebf023e28c7c6ccfd140456e633d8c3386465ae6acf1d285a9459c071a5b167e8c6b9342227370a149a97c5f43bbef3413102ad97ed6500251a24cb02724f171151ad4b980640dde3a28d00ddb13b1a8880e22e32e0b12fde8f4393580734a79165d138e4332a7464991f0da8971015ebeba384a5e45b3927947b53968b0bc265f02fa2670dccc56d1f613cf84cbc9a5350ceea5b2e9094449e0937fa905bd01c9096eba512f0bc82bed24cb8113297260d2b6d0efa15e303f10acd18f2fdfdfe3785a076b2a7085a9bbd21d9de80f64804810bdb9a97e3b3f4b3ed942a5e859c616a5d0fe279b6d1952a2ac1c4e08d290c7e0f9ea26eb1a17e07d80b52979c3aab0cc1efa8ab37fd80a261b2852b6db610e0a7c3e7";
    std::string sign = "d13fdb8e3467b19f4af734621ebf2255437e0ed29b971904108e6cf4aae155854689bf886151e4ce419ef7d58eaca015c9ff110874591b3f8014b429ad132e68d7f836f10f8438fc8b7b5fb4a2ef5174c54d16d4c043135f7059942e310c993dbaca1a9f815c63b674d0a19f97256bfbebaa813ed21fa5a97bf8bf0bb476de4ef1fa005486825847b1774a7d4ee84e2b630f071facf3d75361c2573a30e3fb7351aa5d485fd1de681c2b7b4080a04f232b49a24f6897d8a4e0038465caf43ba0e8360ede0216b8c1fb1739421344e4e4935b916e94010e772087c7696d148d18af4e37e3ad99a009545d5793a4f5d46de3f65e93e4f88ae48549aa47cf9a46af";
    MIDEncryption::MIDEncryption midEncryption(mid);
    //midEncryption.ParseData(source, sign, destinationBuffer);
}