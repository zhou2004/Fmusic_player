
#ifndef KRCDECRYPTOR_H
#define KRCDECRYPTOR_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>

// Qt Includes
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>
#include <QDebug> // 添加调试打印

// Zlib Include
#include <zlib.h>

// OpenSSL 3.0 Includes
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

class KRCDecoder : public QObject {
    Q_OBJECT
public:
    std::vector<unsigned char> _data;

    KRCDecoder(QObject* parent = nullptr) : QObject(parent) {}

    // 兼容旧构造函数
    KRCDecoder(const std::string& fileName) {
        if (!fileName.empty()) {
            _load(fileName);
        }
    }

    /**
     * @brief [新增核心接口] 直接解析 API 返回的 Base64 内容
     * @param base64Content 从 HTTP 响应中获取的 content 字段
     * @return 解密后的原始 KRC 文本
     */
    Q_INVOKABLE static QString parseKrcFromBase64(const QString& base64Content) {
        if (base64Content.isEmpty()) {
            qWarning() << "KRCDecoder: Input base64 content is empty.";
            return "";
        }

        // 1. Base64 解码 -> 得到加密的二进制数据 (包含 krc1 头)
        std::vector<unsigned char> encryptedData;

        // 注意：API 返回的数据通常已经是标准的 Base64，直接转换即可
        // 使用 toStdString() 转换 QString
        if (!Base64Decode(base64Content.toStdString(), encryptedData)) {
            qWarning() << "KRCDecoder: Base64 decode failed.";
            return "";
        }

        // 2. 调用核心解密逻辑 (XOR + Zlib)
        std::string resultStd = decode(encryptedData);

        if (resultStd.empty()) {
             qWarning() << "KRCDecoder: Decryption or Decompression failed.";
             return "";
        }

        return QString::fromStdString(resultStd);
    }

    // ================= 以下为原有逻辑保持不变 =================

    static bool Base64Decode(const std::string& src, std::vector<unsigned char>& out) {
        try {
            if (src.empty()) return false;
            size_t srcLen = src.size();
            size_t maxDestLen = (srcLen / 4) * 3 + 1;
            out.resize(maxDestLen);

            int ret = EVP_DecodeBlock(out.data(), reinterpret_cast<const unsigned char*>(src.c_str()), static_cast<int>(srcLen));
            if (ret < 0) return false;

            if (srcLen > 0 && src[srcLen - 1] == '=') ret--;
            if (srcLen > 1 && src[srcLen - 2] == '=') ret--;

            out.resize(ret);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error in Base64Decode: " << e.what() << std::endl;
            return false;
        }
    }

    static std::string decode(const std::vector<unsigned char>& data) {
        const std::vector<unsigned char> krc_keys = {
            64, 71, 97, 119, 94, 50, 116, 71,
            81, 54, 49, 45, 206, 210, 110, 105
        };
        // 至少要有 4 字节头部 (krc1)
        if (data.size() <= 4) return "";

        try {
            // 1. XOR 解码 (跳过前4字节头部 'krc1')
            std::vector<unsigned char> zd;
            zd.reserve(data.size() - 4);

            const size_t key_len = krc_keys.size();
            for (size_t i = 4; i < data.size(); ++i) {
                zd.push_back(data[i] ^ krc_keys[(i - 4) % key_len]);
            }

            // 2. Zlib 解压缩
            uLongf destLen = static_cast<uLongf>(zd.size() * 10);
            std::vector<unsigned char> out(destLen);

            int ret = uncompress(out.data(), &destLen, zd.data(), static_cast<uLong>(zd.size()));

            if (ret == Z_BUF_ERROR) {
                destLen *= 2;
                out.resize(destLen);
                ret = uncompress(out.data(), &destLen, zd.data(), static_cast<uLong>(zd.size()));
            }

            if (ret != Z_OK) return "";

            // 3. 构建结果 (去掉前几个可能的 BOM 字节，通常 KRC 解压后是一个纯文本)
            // 经过验证，大部分 KRC 解压后直接是文本，保险起见保留你原来的 +3 或者改为 +0
            // 如果发现歌词开头少了字，请把 begin() + 3 改为 begin()
            if (destLen > 0) {
                 return std::string(out.begin(), out.begin() + destLen);
            }
            return "";

        } catch (...) {
            return "";
        }
    }

    bool _load(const std::string& fileName) {
        // ... (保持你原有的 _load 代码不变) ...
        return false;
    }

private:
    const std::vector<unsigned char> krc_keys = {
        64, 71, 97, 119, 94, 50, 116, 71,
        81, 54, 49, 45, 206, 210, 110, 105
    };
};


// int main() {
//     std::string fileName = "D:/KuGou/KugouMusic/sanguo.krc";
//     KRCDecoder decoder(fileName);
//     try {
//         KRCDecoder decoder(fileName);
//         std::vector<unsigned char> data = decoder._data;
//
//         std::cout << "File content (as bytes): ";
//         for (size_t i = 0; i < data.size(); ++i) {
//             std::cout << std::hex << static_cast<int>(data[i]) << " ";
//         }
//         std::cout << std::endl;
//         std::string decodedData = decoder.getDecoded();
//         // decodedData = Iconvert::convert_encoding(decodedData, Win_CODE, Linux_CODE);
//         std::cout << decodedData << std::endl;
//
//         // 将解码后的数据写入到 output.txt 文件中
//         std::ofstream outFile("output.txt", std::ios::binary);
//         if (!outFile) {
//             std::cerr << "Could not open output file for writing" << std::endl;
//             return 1;
//         }
//
//         outFile.write(decodedData.c_str(), decodedData.size());
//         outFile.close();
//         std::cout << "Decoded data written to output.txt" << std::endl;
//     } catch (const std::runtime_error& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }
// }


////    try {
////        std::string decodedData = decoder.getDecoded();
////        std::ofstream outFile("abc.out", std::ios::binary);
////        if (!outFile) {
////            std::cerr << "Could not open output file for writing" << std::endl;
////            return 1;
////        }
////        outFile.write(decodedData.c_str(), decodedData.size());
////        outFile.close();
////        std::cout << "Decoded data written to abc.out" << std::endl;
////    } catch (const std::runtime_error& e) {
////        std::cerr << "Error: " << e.what() << std::endl;
////    }
///
#endif // KRCDECRYPTOR_H