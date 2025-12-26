#pragma once

#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <QString>
#include <fstream>

// 判断是否存在某元素
template <typename T>
bool has(const std::vector<T>& list, const T& obj) {
    for (const auto& item : list) {
        if (item == obj) { return true; }
    }
    return false;
}

// 音乐信息
struct musicInfo {
    std::string musicName;
    std::string format;
    std::vector<std::string> artist;
    std::string cover;
    std::string ncmkey;
    std::string album;
};

// FLAC 文件头
constexpr char FLAC_HEADER[4] = { 'f', 'L', 'a', 'C' };
constexpr char MP3_HEADER[4] = { 'I', 'D', '3', '\0' };

// 搜索文件
inline std::vector<std::filesystem::path> SerchFiles(const std::filesystem::path& Dir, const std::vector<std::string>& Suffixs) {
    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(Dir)) {
        if (std::filesystem::is_regular_file(entry.path()) && has(Suffixs, entry.path().extension().string())) {
            files.push_back(entry.path());
        }
    }
    return files;
}

// 替换文本
inline std::string replace_(std::string tarstr, const std::string& oldstr, const std::string& newstr) {
    std::regex reg(oldstr);
    std::string replaced_str = std::regex_replace(tarstr, reg, newstr);
    return replaced_str;
}

// 拼接数组
template<typename T>
std::string join(const T& list, const std::string& split) {
    std::string out;
    for (size_t i = 0; i < list.size() - 1; i++) {
        out += list[i] + split;
    }
    out += list[list.size() - 1];
    return out;
}

template<typename T>
QString Qjoin(const T& list, const std::string& split) {
    QString out;
    for (size_t i = 0; i < list.size() - 1; i++) {
        out += QString::fromUtf8(list[i]) + QString::fromUtf8(split);
    }
    out += QString::fromUtf8(list[list.size() - 1]);
    return out;
}

// 创建临时文件
inline std::filesystem::path CreateTempFile(const std::filesystem::path& indir) {
    int pos = 100;
    std::string pat = "";
    std::string org = indir.string();
    if (org.back() == '\\' || org.back() == '/') {
        pat = indir.string() + "temp_";
    } else {
        pat = indir.string() + "\\" + "temp_";
    }
    std::ifstream file(pat + std::to_string(pos));
    while (file) {
        pos += 1;
        file = std::ifstream(pat + std::to_string(pos));
    }
    return (pat + std::to_string(pos));
}