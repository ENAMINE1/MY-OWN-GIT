#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <openssl/sha.h>
#include <zlib.h>
#include <map>
#include "write_tree.h"
#include "hash_object.h"

// The git write - tree command creates a tree object from the current state of the "staging area".The staging area is a place where changes go when you run git add. The staging area is also known as the "index".The tree object represents the state of the working directory at a particular point in time. It contains the names of files and directories, along with their modes and hashes.

// A tree object will contain the state of working directory in the form of other tree and blob objects
std::string write_tree(const std::string &directory)
{
    // std::cout << "Writing tree for directory: " << directory << std::endl;
    std::vector<std::string> tree_entries;
    std::vector<std::string> skip;
    // = {
    //     ".git", "server", "CMakeCache.txt",
    //     "CMakeFiles", "Makefile", "cmake_install.cmake"};
    // read the .gitignore file and store the entries in the skip vector
    std::ifstream gitignore(".gitignore");
    if (gitignore.is_open())
    {
        std::string line;
        while (std::getline(gitignore, line))
        {
            skip.push_back(line);
        }
    }

    for (const auto &entry : std::filesystem::directory_iterator(directory))
    {
        std::string path = entry.path().string();

        if (std::any_of(skip.begin(), skip.end(), [&path](const std::string &s)
                        { return path.find(s) != std::string::npos; }))
        {
            continue;
        }
        std::string entry_type;
        std::error_code ec;
        if (std::filesystem::is_directory(entry.status()))
        {
            entry_type = "040000 "; // Directory
        }
        else
        {
            // Determine file permissions
            auto perms = std::filesystem::status(entry).permissions();
            entry_type = check_file_perms(perms);
        }
        std::string relative_path = path.substr(path.find(directory) + directory.length() + 1);
        std::string hash = std::filesystem::is_directory(path, ec) ? write_tree(path.c_str()) : hash_object(path.c_str());
        std::string object_type;
        // if (entry_type == "040000 ")
        //     object_type = "tree";
        // else
        //     object_type = "blob";
        // std::cerr << entry_type + ' ' << object_type << ' ' + hash + '\t' + relative_path << std::endl;

        tree_entries.emplace_back(path + '\0' + entry_type + relative_path + '\0' + hash);
    }
    // sort the entries based on the absolute path O(nlogn)
    std::sort(tree_entries.begin(), tree_entries.end());
    // delete the path from the beginning of each entry
    int bytes = 0;
    /*
    example of entry of a tree object
    "tree"+bytes+'\0'+entry1+'\0'+entry2+'\0'+entry3
    where each entry is of the form
    100644 file.txt\0<20-byte SHA-1 hash for file.txt> "OR"
    040000 dir\0<20-byte SHA-1 hash for the tree object "dir">
    */
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
    // std::cerr << tree_content << std::endl;
    // storing the tree object
    std::string tree_hash = compute_sha1(tree_content, false);
    compress_and_store(tree_hash.c_str(), tree_content, ".");
    // std::cout << "Tree hash for :" << directory << tree_hash << std::endl;
    return tree_hash;
}

std::string check_file_perms(std::filesystem::perms file_perms)
{
    using std::filesystem::perms;
    std::string ret = "100000 ";

    if((perms::none != (file_perms & perms::owner_exec)))
        ret[3] += 1;
    if((perms::none != (file_perms & perms::group_exec)))
        ret[4] += 1;
    if((perms::none != (file_perms & perms::others_exec)))
        ret[5] += 1;
    if((perms::none != (file_perms & perms::owner_write)))
        ret[3] += 2;
    if((perms::none != (file_perms & perms::group_write)))
        ret[4] += 2;
    if((perms::none != (file_perms & perms::others_write)))
        ret[5] += 2;
    if((perms::none != (file_perms & perms::owner_read)))
        ret[3] += 4;
    if((perms::none != (file_perms & perms::group_read)))
        ret[4] += 4;
    if((perms::none != (file_perms & perms::others_read)))
        ret[5] += 4;
    return ret;
}
