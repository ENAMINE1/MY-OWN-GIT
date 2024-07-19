#ifndef WRITE_TREE_H
#define WRITE_TREE_H
#include <regex>
#include"utils.h"
#include "hash_object.h"

std::string write_tree(const std::string &directory);
std::string check_file_perms(std::filesystem::perms file_perms);
// std::vector<Entry> read_index();

#endif // WRITE_TREE_H
