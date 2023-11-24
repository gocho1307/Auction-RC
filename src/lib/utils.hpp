#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>

typedef struct {
    std::string name;
    int size;
    std::string data;
} FileInfo;

int validatePort(std::string port);

std::string readString(std::string &line, bool ignSpaces);

int readInt(std::string &line, int &num, bool ignSpaces);

int readUID(std::string &line, std::string &uid, bool ignSpaces);

int readPassword(std::string &line, std::string &password, bool ignSpaces);

int readFile(std::string fPath, FileInfo &fInfo);

int writeFile(std::string dir, FileInfo fInfo);

void setupSigHandlers(void (*sigF)(int));

#endif // __UTILS_HPP__
