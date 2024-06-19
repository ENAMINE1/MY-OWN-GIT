#include "git_add.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <openssl/sha.h>
#include <zlib.h>
#include <filesystem>

std::string read_file_contents(const std::string &filepath);

void write_index_header(std::ofstream &index_file, int num_entries)
{
    // Write the index header
    char header[12] = {'D', 'I', 'R', 'C', 0, 0, 0, 2}; // DIRC (in ASCII), version 2
    index_file.write(header, 12);

    // Write the number of entries
    index_file.write((char *)&num_entries, 4);
}

void add_index_entry(std::ofstream &index_file, const std::string &mode, const std::string &hash, const std::string &name)
{
    IndexEntry entry;

    // Copy mode
    std::strncpy(entry.mode, mode.c_str(), sizeof(entry.mode));

    // Copy hash
    std::memcpy(entry.hash, hash.c_str(), sizeof(entry.hash));

    // Set flags (normally 0000 in Git)
    std::memset(entry.flags, 0, sizeof(entry.flags));

    // Set name length
    entry.name_length = static_cast<unsigned short>(name.size());

    // Copy name
    entry.name = name;

    // Write the index entry to the file
    index_file.write(entry.mode, sizeof(entry.mode));
    index_file.write(entry.hash, sizeof(entry.hash));
    index_file.write(entry.flags, sizeof(entry.flags));
    index_file.write((char *)&entry.name_length, sizeof(entry.name_length));
    index_file.write(entry.name.c_str(), entry.name.size());
}

void git_add(const std::string &filepath)
{
    // Open the index file in append mode
    std::ofstream index_file(".git/index", std::ios::binary | std::ios::app);
    if (!index_file)
    {
        std::cerr << "Error: could not open .git/index" << std::endl;
        return;
    }

    // Print the filepath for debugging
    std::cout << "Opening file: " << filepath << std::endl;

    // Retrieve the file name
    std::string filename = std::filesystem::path(filepath).filename().string();
    std::cerr<<filename<<std::endl;
    // Calculate the SHA-1 hash of the file content
    std::string content = read_file_contents(filepath); // Implement this function to read the file content
    std::string hash = sha1_hex(content);

    // Add the entry to the index
    add_index_entry(index_file, "100644", hash, filename);

    // Close the index file
    index_file.close();
}

std::string read_file_contents(const std::string &filepath)
{
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: could not open file " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    file.close(); // Explicitly close the file

    return content;
}