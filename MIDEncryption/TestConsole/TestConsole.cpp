// TestConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../MIDEncryption/RSAEncryption.h"
#include "../MIDEncryption/AESEncryption.h"
#include "../MIDEncryption/MIDEncryption.h"
#include "../MIDEncryption/ShaHash.h"
#include <Windows.h>
#include <wincrypt.h>

using namespace std;

std::string PUBLICKEY = "-----BEGIN PUBLIC KEY-----\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy8Dbv8prpJ/0kKhlGeJY\n"\
"ozo2t60EG8L0561g13R29LvMR5hyvGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+\n"\
"vw1HocOAZtWK0z3r26uA8kQYOKX9Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQAp\n"\
"fc9jB9nTzphOgM4JiEYvlV8FLhg9yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68\n"\
"i6T4nNq7NWC+UNVjQHxNQMQMzU6lWCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoV\n"\
"PpY72+eVthKzpMeyHkBn7ciumk5qgLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUy\n"\
"wQIDAQAB\n"\
"-----END PUBLIC KEY-----\n";

std::string PRIVATEKEY = "-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEAy8Dbv8prpJ/0kKhlGeJYozo2t60EG8L0561g13R29LvMR5hy\n"\
"vGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+vw1HocOAZtWK0z3r26uA8kQYOKX9\n"\
"Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQApfc9jB9nTzphOgM4JiEYvlV8FLhg9\n"\
"yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68i6T4nNq7NWC+UNVjQHxNQMQMzU6l\n"\
"WCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoVPpY72+eVthKzpMeyHkBn7ciumk5q\n"\
"gLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUywQIDAQABAoIBADhg1u1Mv1hAAlX8\n"\
"omz1Gn2f4AAW2aos2cM5UDCNw1SYmj+9SRIkaxjRsE/C4o9sw1oxrg1/z6kajV0e\n"\
"N/t008FdlVKHXAIYWF93JMoVvIpMmT8jft6AN/y3NMpivgt2inmmEJZYNioFJKZG\n"\
"X+/vKYvsVISZm2fw8NfnKvAQK55yu+GRWBZGOeS9K+LbYvOwcrjKhHz66m4bedKd\n"\
"gVAix6NE5iwmjNXktSQlJMCjbtdNXg/xo1/G4kG2p/MO1HLcKfe1N5FgBiXj3Qjl\n"\
"vgvjJZkh1as2KTgaPOBqZaP03738VnYg23ISyvfT/teArVGtxrmFP7939EvJFKpF\n"\
"1wTxuDkCgYEA7t0DR37zt+dEJy+5vm7zSmN97VenwQJFWMiulkHGa0yU3lLasxxu\n"\
"m0oUtndIjenIvSx6t3Y+agK2F3EPbb0AZ5wZ1p1IXs4vktgeQwSSBdqcM8LZFDvZ\n"\
"uPboQnJoRdIkd62XnP5ekIEIBAfOp8v2wFpSfE7nNH2u4CpAXNSF9HsCgYEA2l8D\n"\
"JrDE5m9Kkn+J4l+AdGfeBL1igPF3DnuPoV67BpgiaAgI4h25UJzXiDKKoa706S0D\n"\
"4XB74zOLX11MaGPMIdhlG+SgeQfNoC5lE4ZWXNyESJH1SVgRGT9nBC2vtL6bxCVV\n"\
"WBkTeC5D6c/QXcai6yw6OYyNNdp0uznKURe1xvMCgYBVYYcEjWqMuAvyferFGV+5\n"\
"nWqr5gM+yJMFM2bEqupD/HHSLoeiMm2O8KIKvwSeRYzNohKTdZ7FwgZYxr8fGMoG\n"\
"PxQ1VK9DxCvZL4tRpVaU5Rmknud9hg9DQG6xIbgIDR+f79sb8QjYWmcFGc1SyWOA\n"\
"SkjlykZ2yt4xnqi3BfiD9QKBgGqLgRYXmXp1QoVIBRaWUi55nzHg1XbkWZqPXvz1\n"\
"I3uMLv1jLjJlHk3euKqTPmC05HoApKwSHeA0/gOBmg404xyAYJTDcCidTg6hlF96\n"\
"ZBja3xApZuxqM62F6dV4FQqzFX0WWhWp5n301N33r0qR6FumMKJzmVJ1TA8tmzEF\n"\
"yINRAoGBAJqioYs8rK6eXzA8ywYLjqTLu/yQSLBn/4ta36K8DyCoLNlNxSuox+A5\n"\
"w6z2vEfRVQDq4Hm4vBzjdi3QfYLNkTiTqLcvgWZ+eX44ogXtdTDO7c+GeMKWz4XX\n"\
"uJSUVL5+CVjKLjZEJ6Qc2WZLl94xSwL71E41H4YciVnSCQxVc4Jw\n"\
"-----END RSA PRIVATE KEY-----\n";

void main(void)
{
    //std::vector<uint8_t> publicKeyBuffer(PUBLICKEY.size());
    //memcpy(&publicKeyBuffer[0], &PUBLICKEY[0], PUBLICKEY.size());
    //std::vector<uint8_t> privateKeyBuffer(PRIVATEKEY.size());
    //memcpy(&privateKeyBuffer[0], &PRIVATEKEY[0], PRIVATEKEY.size());
    std::vector<uint8_t> mid = { '1', '1','1', '1', '1', '1'};
    {
        std::string source = "\"11111111111111\"";
        std::vector<uint8_t> sourceBuffer(source.size());
        memcpy(&sourceBuffer[0], &source[0], source.size());
        std::vector<uint8_t> destinationBuffer;

        //MIDEncryption::RSAEncryption::PrivateEncrypt(sourceBuffer, destinationBuffer, privateKeyBuffer);
        //MIDEncryption::RSAEncryption::PublicDecrypt(destinationBuffer, sourceBuffer, publicKeyBuffer);

        std::vector<uint8_t> iv = { 0x29, 0x43, 0x00, 0x4d, 0x3b, 0xb1, 0xc5, 0x7c, 0xff, 0xd8, 0x83, 0xc1, 0xe8, 0xd0, 0x75, 0xf3 };
        MIDEncryption::AESEncryption::Encrypt(sourceBuffer, destinationBuffer, mid, iv);
        MIDEncryption::AESEncryption::CBCEncrypt(sourceBuffer, destinationBuffer, mid, iv);
        //MIDEncryption::AESEncryption::CBCDecrypt(destinationBuffer, sourceBuffer, mid, iv);
    }

    //std::string source = "89e1b9ae21307b773357be846834676782252d9c97efec685b654796c9c024e53587bd756cc484bce114be7862ad1f3cff5cb9dfc4fb31ace3960ceb3c637bc562b307d3cba2df6b21f0b35c2627b461fdcfc6fc17114b9b8b02ef88c15c7b75f56e5fddcd07cb66647aad3ed53468dd1b4f0dfa977d7b563b073f5d8b8cc1f9557a00f21f03054e1b1c67dd60eb0d31ea69a2268939c3b5255506d577e39f8782366c7fac63ebdbe6ce3023a16dfb863deb71935018911d1715db1332dbacb5d131bce8f2c17a188582821b58efc5da984d167556629f9a04a36a86c355f0b858a677c1930cd64fb4489e2f197d124bcc9d2ddb23ed5c987ea27dc4f3106be295f875408f9b5204b8f679ddc5d1089dcca8fcb5d48826d254fa50887bee3cb0";
    //std::string sign = "203ddeeeb99ac86764c3b7a836c2146cbc8a83a41c02e60ec584dac2b1213a1fda7e08238b39f68d4df4ae4d27b9424f3406c80e3fd66ba8b3363beb7dd6979b676795788e3c9d4c26eab55af4dcc83cd66dc10b8dcd0b1de70b4903562fcc5a6fea6561445a48797c2d148c8adc1363eb6b043239fc26b461f826edc4fa093b856d868c7a4e62841bf730865412c0b851aae7c99d487d81aa4d187029cb6bc1982a8775d51348c63c4d1807279261897244d1ed37c8af1c64222256f19b5b4e0b5f985988ffedceb791357822e2241b3b73ffaf9734e244e388741155dc6fa38f1a0a0111d6dde5706d978041a17df74afef20e8d0c447398e17246d8cb19ac";
    //MIDEncryption::MIDEncryption midEncryption(mid);
    //std::vector<uint8_t> destinationBuffer;
    //midEncryption.ParseData(source, sign, destinationBuffer);
}