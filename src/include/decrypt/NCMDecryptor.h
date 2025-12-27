#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cstring>
#include <algorithm>
#include "Cde.h" // 必须包含：提供 aes_ecb_decrypt 函数

class NCM
{
private:
    // NCM 核心 AES 密钥 (用于解密 RC4 Key)
    constexpr static const unsigned char CORE_KEY[] = { 0x68, 0x7A, 0x48, 0x52, 0x41, 0x6D, 0x73, 0x6F, 0x35, 0x6B, 0x49, 0x6E, 0x62, 0x61, 0x78, 0x57 };

    // 文件头标识
    constexpr static const unsigned char NCM_HEADER_MAGIC[] = { 0x43, 0x54, 0x45, 0x4e, 0x46, 0x44, 0x41, 0x4d };

    // 格式探测头
    constexpr static char FLAC_HEAD[4] = { 'f', 'L', 'a', 'C' };
    constexpr static char MP3_HEAD[3] = { 'I', 'D', '3' };

    // ----------------------------------------------------------------------
    // [静态辅助函数]
    // ----------------------------------------------------------------------

    /**
     * @brief 校验是否为 NCM 文件
     * 读取前 8 字节对比 CTENFDAM，并跳过随后的 2 字节空隙
     */
    static bool CheckNCMHeader(std::ifstream& inFile) {
        char magic[10]; // 读取 8字节头 + 2字节保留位
        inFile.read(magic, 10);

        if (inFile.gcount() < 10) return false;

        // 对比前 8 字节
        if (std::memcmp(magic, NCM_HEADER_MAGIC, 8) != 0) {
            return false;
        }
        return true;
    }

    /**
     * @brief 构建 NCM 专用的 KeyBox (非标准 RC4)
     * @param keybox 输出的 256 字节 S-Box
     * @param key 解密后的 RC4 密钥
     * @param keyLen 密钥长度
     */
    static void BuildKeyBox(unsigned char* keybox, const unsigned char* key, int keyLen) {
        // 1. 初始化 0-255
        for (int i = 0; i < 256; ++i) {
            keybox[i] = (unsigned char)i;
        }

        // 2. NCM 专用的打乱逻辑
        unsigned char last_byte = 0;
        unsigned char key_offset = 0;

        for (int i = 0; i < 256; ++i) {
            unsigned char swap = keybox[i];
            // 核心差异：引入了 last_byte 参与运算
            unsigned char c = (swap + last_byte + key[key_offset]) & 0xff;

            key_offset++;
            if (key_offset >= keyLen) key_offset = 0;

            keybox[i] = keybox[c];
            keybox[c] = swap;
            last_byte = c;
        }
    }

public:
    /**
     * @brief 静态 NCM 解密函数
     * @param originalFilePath 源文件路径
     * @param _outputPath 输出目录 (可选)
     * @return 成功返回文件的绝对路径字符串，失败返回 ""
     */
    static std::string NCMDecrypt(const std::filesystem::path& originalFilePath, std::filesystem::path _outputPath = "")
    {
        namespace fs = std::filesystem;

        try {
            std::ifstream inFile(originalFilePath, std::ios::binary);
            if (!inFile) return "";

            // 1. 校验文件头 "CTENFDAM"
            unsigned int header[2];
            inFile.read(reinterpret_cast<char*>(header), 8);
            if (header[0] != 0x4e455443 || header[1] != 0x4d414446) { // CTEN 和 FDAM
                return "";
            }
            inFile.seekg(2, std::ios::cur); // 跳过 2 字节保留位

            // 2. 读取 Key 长度
            unsigned int keyLen = 0;
            inFile.read(reinterpret_cast<char*>(&keyLen), 4);
            if (keyLen == 0) return "";

            // 3. 读取 Key 数据并预处理
            std::vector<char> rawKeyData(keyLen);
            inFile.read(rawKeyData.data(), keyLen);
            for (unsigned int i = 0; i < keyLen; i++) {
                rawKeyData[i] ^= 0x64; // NCM Key 混淆去除
            }

            // 4. AES 解密 Key
            // 构造 CoreKey 字符串
            std::string coreKeyStr(reinterpret_cast<const char*>(CORE_KEY), 16);
            std::string rawKeyString(rawKeyData.begin(), rawKeyData.end());

            // 调用 Cde.h 中的 AES 解密
            std::string decryptedKey = aes_ecb_decrypt(rawKeyString, coreKeyStr);

            // 去除前 17 字节 ("neteasecloudmusic")
            if (decryptedKey.length() <= 17) return "";
            const unsigned char* finalKey = reinterpret_cast<const unsigned char*>(decryptedKey.data() + 17);
            int finalKeyLen = decryptedKey.length() - 17;

            // 5. 构建 KeyBox
            unsigned char keyBox[256];
            BuildKeyBox(keyBox, finalKey, finalKeyLen);

            // 6. 智能跳过元数据 (Info Chunk)
            unsigned int infoLen = 0;
            inFile.read(reinterpret_cast<char*>(&infoLen), 4);
            inFile.seekg(infoLen, std::ios::cur); // 跳过 JSON

            // 7. 跳过 CRC (4) + Gap (5)
            inFile.seekg(9, std::ios::cur);

            // 8. 智能跳过封面 (Image Chunk)
            unsigned int imgLen = 0;
            inFile.read(reinterpret_cast<char*>(&imgLen), 4);
            inFile.seekg(imgLen, std::ios::cur); // 跳过图片

            // --------------------------------------------------
            // 准备解密输出
            // --------------------------------------------------
            if (_outputPath.empty()) {
                _outputPath = originalFilePath.parent_path();
            }
            if (!fs::exists(_outputPath)) {
                fs::create_directories(_outputPath);
            }

            const size_t BUFFER_SIZE = 1024 * 64; // 64KB 缓冲区
            std::vector<unsigned char> buffer(BUFFER_SIZE);

            std::ofstream outFile;
            std::string resultPathString = "";
            bool firstBlock = true;

            // 维护全局文件偏移量 (音乐数据部分的)
            // 注意：NCM 的异或逻辑依赖于字节在音乐数据中的绝对位置
            size_t musicDataOffset = 0;

            // 9. 流式解密循环
            while (inFile) {
                inFile.read(reinterpret_cast<char*>(buffer.data()), BUFFER_SIZE);
                std::streamsize bytesRead = inFile.gcount();
                if (bytesRead <= 0) break;

                // NCM 异或逻辑 (移植自 decrypt_handler.hpp)
                for (std::streamsize k = 0; k < bytesRead; ++k) {
                    // j 是基于 (当前绝对位置 + 1) 的低8位
                    unsigned char j = (unsigned char)((musicDataOffset + k + 1) & 0xff);

                    // NCM 魔法异或公式
                    unsigned char box_j = keyBox[j]; // keybox[j]
                    unsigned char box_next = keyBox[(box_j + j) & 0xff]; // keybox[(keybox[j] + j) & 0xff]
                    unsigned char mask = keyBox[(box_j + box_next) & 0xff];

                    buffer[k] ^= mask;
                }

                // 更新全局偏移量
                musicDataOffset += bytesRead;

                // 10. 格式探测 (仅第一次)
                if (firstBlock) {
                    std::string ext = ".mp3"; // 默认为 mp3
                    // 检测 FLAC
                    if (bytesRead >= 4 && std::memcmp(buffer.data(), FLAC_HEAD, 4) == 0) {
                        ext = ".flac";
                    }
                    // 检测 MP3 (ID3)
                    else if (bytesRead >= 3 && std::memcmp(buffer.data(), MP3_HEAD, 3) == 0) {
                        ext = ".mp3";
                    }

                    fs::path outputFileName = originalFilePath.stem();
                    outputFileName += ext;
                    fs::path fullOutputPath = _outputPath / outputFileName;

                    outFile.open(fullOutputPath, std::ios::binary);
                    if (!outFile) return "";

                    resultPathString = fullOutputPath.string();
                    firstBlock = false;
                }

                outFile.write(reinterpret_cast<char*>(buffer.data()), bytesRead);
            }

            outFile.close();
            inFile.close();
            return resultPathString;

        } catch (...) {
            return "";
        }
    }
};