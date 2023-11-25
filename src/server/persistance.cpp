#include "persistance.hpp"
#include "../lib/messages.hpp"

#include <filesystem>
#include <fstream>

int readFile(std::string fPath, FileInfo &fInfo) {
    (void)fPath;
    (void)fInfo;
    // std::ifstream file(fPath, std::ios::in | std::ios::binary);
    // if (!file.is_open() || !file.good()) {
    //     std::cerr << FILE_ERR << std::endl;
    //     return -1;
    // }
    //
    // std::filesystem::path path(fPath);
    // std::string name = path.filename();
    // unsigned long int size = std::filesystem::file_size(path);
    // for (char c : name) {
    //     if (!isalnum(c) && c != '-' && c != '_' && c != '.') {
    //         std::cerr << FILE_NAME_ERR << std::endl;
    //         return -1;
    //     }
    // }
    // if (size <= 0 || size >= MAX_FILE_SIZE) {
    //     std::cerr << FILE_SIZE_ERR << std::endl;
    //     return -1;
    // }
    // fInfo.name = name;
    // fInfo.size = (int)size;
    //
    // char buffer[BUFFER_FILE_SIZE] = {0};
    // while (file) {
    //     file.read(buffer, BUFFER_FILE_SIZE);
    //     fInfo.data += std::string(buffer);
    // }
    // file.close();
    return 0;
}

int writeFile(std::string dir, FileInfo fInfo) {
    (void)dir;
    (void)fInfo;
    // std::ofstream file(dir + fInfo.name);
    // if (!file.is_open() || !file.good()) {
    //     std::cerr << FILE_ERR << std::endl;
    //     return -1;
    // }
    //
    // file.write(fInfo.data.c_str(), fInfo.size);
    // if (!file.good()) {
    //     file.close();
    //     std::cerr << FILE_ERR << std::endl;
    //     return -1;
    // }
    // file.close();
    return 0;
}
