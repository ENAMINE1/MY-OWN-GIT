#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <zlib.h>
#include <openssl/sha.h>
#include "hash_object.h"
#include "utils.h"

void hash_object(const std::string &file)
{
    // Read file contents
    std::ifstream t(file);
    std::stringstream data;
    data << t.rdbuf();
    // Create blob content string
    std::string content = "blob " + std::to_string(data.str().length()) + '\0' + data.str();
    // Calculate SHA1 hash
    std::string buffer = sha1_hex(content);
    // Compress blob content
    uLong bound = compressBound(content.size());
    unsigned char compressedData[bound];
    compressFile(content, &bound, compressedData);
    // Write compressed data to .git/objects
    std::string dir = ".git/objects/" + buffer.substr(0, 2);
    std::filesystem::create_directory(dir);
    std::string objectPath = dir + "/" + buffer.substr(2);
    std::ofstream objectFile(objectPath, std::ios::binary);
    objectFile.write((char *)compressedData, bound);
    objectFile.close();
}
