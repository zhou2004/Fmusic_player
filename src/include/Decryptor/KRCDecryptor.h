
// #ifndef KRCDECRYPTOR_H
// #define KRCDECRYPTOR_H

// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <string>
// #include <zlib.h>
// #include "MusicPlayer.h"
// #include <QObject>
// #include <sstream>
// #include <iomanip>
// #include <QNetworkAccessManager>
// #include <QNetworkReply>
// #include <QEventLoop>

// #include <openssl/bio.h>
// #include <openssl/evp.h>
// #include <openssl/buffer.h>
// /*
// 1.跳过输入数据的前 4 个字节（文件头）。
// 2.对剩余的数据进行 XOR 解码，使用 krc_keys 数组。
// 3.对解码后的数据进行 zlib 解压缩。
// 4.将解压缩后的数据转换为字符串，并去掉第一个字符。
// */

// class KRCDecoder  : public QObject {
//     Q_OBJECT
// public:
//     std::vector<unsigned char> _data;
//     KRCDecoder(const std::string& fileName = "") {
//         // _load(fileName);
//     }

//     static bool Base64Decode(const std::string& src, std::vector<unsigned char>& out) {
//         try {
//             if (src.empty()) {
//                 // 如果输入为空，直接返回 false
//                 return false;
//             }

//             size_t srcLen = src.size();
//             if (srcLen % 4 != 0) {
//                 // Base64 编码的长度必须是 4 的倍数
//                 return false;
//             }

//             size_t destLen = (srcLen / 4) * 3;
//             out.resize(destLen);

//             int ret = EVP_DecodeBlock((unsigned char*)out.data(), (const unsigned char*)src.c_str(), (int)src.size());
//             if (ret == -1) {
//                 // Base64 解码失败
//                 return false;
//             }

//             int i = 0;
//             while (srcLen > 0 && src.at(--srcLen) == '=') {
//                 ret--;
//                 if (++i > 2) {
//                     // 输入可能不是有效的 Base64 字符串
//                     return false;
//                 }
//             }

//             out.resize(ret);
//             return true;
//         } catch (const std::exception& e) {
//             // 捕获并处理异常
//             std::cerr << "Error in Base64Decode function: " << e.what() << std::endl;
//             return false;
//         }
//     }

//     // Base64 解码函数
//     static std::string base64_decode(const std::string& encoded_string) {
//         BIO *bio, *b64;
//         BUF_MEM *bptr;

//         // 创建一个 Base64 解码的 BIO 链
//         b64 = BIO_new(BIO_f_base64());
//         bio = BIO_new_mem_buf(encoded_string.c_str(), -1);
//         bio = BIO_push(b64, bio);

//         // 设置 BIO 为解码模式
//         BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // 忽略换行符

//         // 创建一个缓冲区来存储解码后的数据
//         BUF_MEM_grow(bptr, encoded_string.size());
//         BIO_get_mem_ptr(bio, &bptr);

//         // 解码数据
//         char* decoded_data = (char*)malloc(bptr->length);
//         int decoded_len = BIO_read(bio, decoded_data, bptr->length);

//         // 清理
//         BIO_free_all(bio);

//         // 返回解码后的字符串
//         return std::string(decoded_data, decoded_len);
//     }

// #include <iostream>
// #include <vector>
// #include <string>
// #include <stdexcept>
// #include <zlib.h> // 确保链接 zlib 库

//     std::string decode(const std::vector<unsigned char>& data) {
//         try {
//             // XOR 解码
//             std::vector<unsigned char> zd;
//             for (size_t i = 4; i < data.size(); ++i) {
//                 zd.push_back(data[i] ^ krc_keys[(i - 4) % 16]);
//             }

//             // 解压缩
//             uLongf destLen = zd.size() * 10; // 输出缓冲区，大小为输入的两倍
//             std::vector<unsigned char> out(destLen);

//             int ret = uncompress(out.data(), &destLen, zd.data(), zd.size());
//             if (ret != Z_OK) {
//                 throw std::runtime_error("Decompression failed");
//             }

//             // 去掉前三个字符
//             std::string result(out.begin() + 3, out.begin() + destLen);

//             // 清理临时数据
//             std::vector<unsigned char>().swap(out);
//             std::vector<unsigned char>().swap(zd);

//             return result;
//         } catch (const std::exception& e) {
//             // 捕获并处理异常
//             std::cerr << "Error in decode function: " << e.what() << std::endl;
//             return ""; // 返回空字符串或适当的错误值
//         }
//     }

//     bool _load(const std::string& fileName) {
//         if (fileName.find("http://")  == 0 || fileName.find("https://")  == 0) {
//             // 处理网络 URL
//             QNetworkAccessManager manager;
//             QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(QString::fromStdString(fileName))));

//             QEventLoop loop;
//             QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
//             loop.exec();

//             if (reply->error() == QNetworkReply::NoError) {
//                 QByteArray data = reply->readAll();
//                 _data.assign(data.constData(), data.constData() + data.length());
//             } else {
//                 std::cerr << "网络请求失败: " << reply->errorString().toStdString() << std::endl;
//                 return false;
//             }
//             reply->deleteLater();
//             return true;
//         }else {
//             std::ifstream file(fileName, std::ios::binary);
//             if (!file) {
//                 std::cerr << "Could not open file: " << fileName << std::endl;
//                 return false;
//             }

//             // 获取文件大小
//             file.seekg(0, std::ios::end);  // 移动到文件末尾
//             std::streamsize fileSize = file.tellg();  // 获取文件大小
//             file.seekg(0, std::ios::beg);  // 移动到文件开头

//             // 调整数据缓冲区大小
//             _data.resize(static_cast<size_t>(fileSize));

//             // 读取文件内容
//             if (!file.read(reinterpret_cast<char*>(_data.data()), fileSize)) {

//                 std::cerr << "Could not read file: " << fileName << std::endl;
//                 return false;
//             }

//             file.close();  // 关闭文件
//             return true;
//         }



//     }

//     std::string getDecoded() {
//         return decode(_data);
//     }

// private:

//     std::vector<unsigned char> krc_keys= {64, 71, 97, 119, 94, 50, 116, 71, 81, 54, 49, 45, 206, 210, 110, 105};
// };


// // int main() {
// //     std::string fileName = "D:/KuGou/KugouMusic/sanguo.krc";
// //     KRCDecoder decoder(fileName);
// //     try {
// //         KRCDecoder decoder(fileName);
// //         std::vector<unsigned char> data = decoder._data;
// //
// //         std::cout << "File content (as bytes): ";
// //         for (size_t i = 0; i < data.size(); ++i) {
// //             std::cout << std::hex << static_cast<int>(data[i]) << " ";
// //         }
// //         std::cout << std::endl;
// //         std::string decodedData = decoder.getDecoded();
// //         // decodedData = Iconvert::convert_encoding(decodedData, Win_CODE, Linux_CODE);
// //         std::cout << decodedData << std::endl;
// //
// //         // 将解码后的数据写入到 output.txt 文件中
// //         std::ofstream outFile("output.txt", std::ios::binary);
// //         if (!outFile) {
// //             std::cerr << "Could not open output file for writing" << std::endl;
// //             return 1;
// //         }
// //
// //         outFile.write(decodedData.c_str(), decodedData.size());
// //         outFile.close();
// //         std::cout << "Decoded data written to output.txt" << std::endl;
// //     } catch (const std::runtime_error& e) {
// //         std::cerr << "Error: " << e.what() << std::endl;
// //     }
// // }


// ////    try {
// ////        std::string decodedData = decoder.getDecoded();
// ////        std::ofstream outFile("abc.out", std::ios::binary);
// ////        if (!outFile) {
// ////            std::cerr << "Could not open output file for writing" << std::endl;
// ////            return 1;
// ////        }
// ////        outFile.write(decodedData.c_str(), decodedData.size());
// ////        outFile.close();
// ////        std::cout << "Decoded data written to abc.out" << std::endl;
// ////    } catch (const std::runtime_error& e) {
// ////        std::cerr << "Error: " << e.what() << std::endl;
// ////    }
// ///
// #endif // KRCDECRYPTOR_H