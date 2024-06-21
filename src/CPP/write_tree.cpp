#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <openssl/sha.h>
#include <zlib.h>
#include "write_tree.h"

// The git write - tree command creates a tree object from the current state of the "staging area".The staging area is a place where changes go when you run git add. The staging area is also known as the "index".The tree object represents the state of the working directory at a particular point in time. It contains the names of files and directories, along with their modes and hashes.

// A tree object will contain the state of working directory in the form of other tree and blob objects

void write_tree()
{
    // Read the index file
    std::vector<TreeEntry> entries = read_index();

    // Sort entries by name
    std::sort(entries.begin(), entries.end(), [](const TreeEntry &a, const TreeEntry &b)
              { return a.name < b.name; });

    // Prepare the tree content
    std::stringstream tree_content;
    for (const TreeEntry &entry : entries)
    {
        tree_content << entry.mode << ' ' << entry.name << '\0' << hash_digest(entry.hash);
    }

    // Compute the SHA-1 hash of the tree object
    std::string tree_data = tree_content.str();
    std::string tree_hash = hash_object(tree_data, "tree");

    std::cout << "Tree object hash: " << tree_hash << std::endl;
}

std::vector<TreeEntry> read_index()
{
    std::vector<TreeEntry> entries;

    // Read the index file
    std::ifstream index_file(".git/index", std::ios::binary);
    if (!index_file)
    {
        std::cerr << "Error: could not open .git/index" << std::endl;
        return entries;
    }

    // Read the index header
    char header[12];
    index_file.read(header, 12);
    if (std::string(header, 4) != "DIRC")
    {
        std::cerr << "Error: invalid index file format" << std::endl;
        return entries;
    }

    // Read the number of index entries
    int num_entries = (header[7] & 0xff) + ((header[6] & 0xff) << 8) + ((header[5] & 0xff) << 16) + ((header[4] & 0xff) << 24);

    // Read each index entry
    for (int i = 0; i < num_entries; ++i)
    {
        TreeEntry entry;

        // Read the mode
        char mode[6];
        index_file.read(mode, 6);
        mode[6] = '\0';
        entry.mode = mode;

        // Read the sha1 hash
        char hash[20];
        index_file.read(hash, 20);
        entry.hash = std::string(hash, 20);

        // Read the file name length
        char name_length[2];
        index_file.read(name_length, 2);
        int length = (name_length[1] & 0xff) + ((name_length[0] & 0xff) << 8);

        // Read the file name
        char name[length + 1];
        index_file.read(name, length);
        name[length] = '\0';
        entry.name = name;

        // Add the entry to the vector
        entries.push_back(entry);
    }

    return entries;
}

std::string hash_object(const std::string &data, const std::string &type)
{
    // Create the content to be hashed
    std::string content = type + " " + std::to_string(data.size()) + '\0' + data;

    // Compute the SHA-1 hash
    std::string hash = sha1_hex(content);

    // Compress the data
    uLong bound = compressBound(content.size());
    unsigned char compressedData[bound];
    compressFile(content, &bound, compressedData);

    // Write the compressed data to .git/objects
    std::string dir = ".git/objects/" + hash.substr(0, 2);
    std::filesystem::create_directory(dir);
    std::string objectPath = dir + "/" + hash.substr(2);
    std::ofstream objectFile(objectPath, std::ios::binary);
    objectFile.write((char *)compressedData, bound);
    objectFile.close();

    return hash;
}
