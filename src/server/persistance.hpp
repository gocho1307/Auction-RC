#ifndef __PERSISTANCE_HPP__
#define __PERSISTANCE_HPP__

#include <string>

typedef struct {
    std::string name;
    std::string data;
} FileInfo;

int readFile(std::string fPath, FileInfo &fInfo);

int writeFile(std::string dir, FileInfo fInfo);

#endif // __PERSISTANCE_HPP__
