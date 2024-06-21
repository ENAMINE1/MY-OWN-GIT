#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <openssl/sha.h>
#include <zlib.h>
#include "write_tree.h"
#include "hash_object.h"

// The git write - tree command creates a tree object from the current state of the "staging area".The staging area is a place where changes go when you run git add. The staging area is also known as the "index".The tree object represents the state of the working directory at a particular point in time. It contains the names of files and directories, along with their modes and hashes.

// A tree object will contain the state of working directory in the form of other tree and blob objects
std::string write_tree(const std::string &directory)
{
    std::vector<std::string> tree_entries;
    std::vector<std::string> skip = {
        ".git", "server", "CMakeCache.txt",
        "CMakeFiles", "Makefile", "cmake_install.cmake"};
    for (const auto &entry : std::filesystem::directory_iterator(directory))
    {
        std::string path = entry.path().string();

        if (std::any_of(skip.begin(), skip.end(), [&path](const std::string &s)
                        { return path.find(s) != std::string::npos; }))
        {
            continue;
        }
        std::error_code ec;
        std::string entry_type = std::filesystem::is_directory(path, ec) ? "040000 " : "100644 ";
        std::string relative_path = path.substr(path.find(directory) + directory.length() + 1);
        std::string hash = std::filesystem::is_directory(path, ec) ? write_tree(path.c_str()) : hash_object(path.c_str());
        std::string object_type;
        if (entry_type == "040000 ")
            object_type = "tree";
        else
            object_type = "blob";
        std::cerr << entry_type + ' ' << object_type << ' ' + hash + '\t' + relative_path << std::endl;

        tree_entries.emplace_back(path + '\0' + entry_type + relative_path + '\0' + hash);
    }
    // sort the entries based on the absolute path
    std::sort(tree_entries.begin(), tree_entries.end());
    // delete the path from the beginning of each entry
    int bytes = 0;
    for (auto &entry : tree_entries)
    {
        entry = entry.substr(entry.find('\0') + 1);
        bytes += entry.length();
    }
    // concatenate the entries
    std::string tree_content = "tree " + std::to_string(bytes) + '\0';
    for (const auto &entry : tree_entries)
    {
        tree_content += entry;
    }
    // storing the tree object
    std::string tree_hash = compute_sha1(tree_content, false);
    compress_and_store(tree_hash.c_str(), tree_content, ".");
    return tree_hash;
}