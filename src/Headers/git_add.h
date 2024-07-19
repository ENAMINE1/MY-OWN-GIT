#ifndef GIT_ADD_H
#define GIT_ADD_H

// https://git-scm.com/docs/index-format

#include"utils.h"
#include"write_tree.h"

void git_add(const std::string &file);

/*
Index entries are sorted in ascending order on the name field,interpreted as a string of unsigned bytes(i.e.memcmp() order, no localization, no special casing of directory separator '/').Entries with the same name are sorted by their stage field.
*/
struct IndexEntry
{
    char mode[6];
    char hash[20];
    char flags[4];
    unsigned short name_length;
    std::string name;
};

#endif // GIT_ADD_H