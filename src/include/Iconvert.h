// //
// // Created by 周俊杰 on 2025/3/23.
// //
//
// #ifndef ICONVERT_H
// #define ICONVERT_H
// #include <string>
// #include <stdexcept>
// #include <iconv.h>
//
// #define Win_CODE "GBK"
// #define Linux_CODE "UTF-8"
//
// class Iconvert {
// public:
//     static std::string convert_encoding(const std::string& input, const char* from, const char* to) {
//         iconv_t cd = iconv_open(to, from);
//         if (cd == (iconv_t)-1) {
//             throw std::runtime_error("iconv_open failed");
//         }
//
//         size_t in_size = input.size();
//         size_t out_size = in_size * 4; // 预留足够空间
//         std::string output(out_size, '\0');
//
//         char* in_ptr = const_cast<char*>(input.data());
//         char* out_ptr = &output[0];
//         size_t result = iconv(cd, &in_ptr, &in_size, &out_ptr, &out_size);
//         iconv_close(cd);
//
//         if (result == (size_t)-1) {
//             throw std::runtime_error("iconv conversion failed");
//         }
//
//         output.resize(output.size()  - out_size); // 调整输出大小
//         return output;
//     }
// };
//
//
//
// #endif //ICONVERT_H
