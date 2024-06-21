#include"utils.h"
#ifndef WRITE_TREE_H
#define WRITE_TREE_H

#include <string>
#include <vector>

void write_tree();
std::vector<TreeEntry> read_index();
std::string hash_object(const std::string &data, const std::string &type);

#endif // WRITE_TREE_H
