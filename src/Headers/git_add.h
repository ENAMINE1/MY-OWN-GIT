#ifndef GIT_ADD_H
#define GIT_ADD_H

#include <string>

void git_add(const std::string &file);

struct IndexEntry
{
    char mode[6];
    char hash[20];
    char flags[4];
    unsigned short name_length;
    std::string name;
};

#endif // GIT_ADD_H