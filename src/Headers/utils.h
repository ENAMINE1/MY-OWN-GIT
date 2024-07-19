#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cstring>
#include <set>
#include <cstdio>
#include <zlib.h>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <zlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// ANSI escape codes for colors
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";

// example of a tree object:
// 100644 blob 3cf56579491f151d82b384c211cf1971c300fbf8 .dockerignore
// 040000 tree 918756f1a4e5d648ae273801359c440c951555f9    build
struct Entry
{
    std::string mode;
    std::string filename;
    std::string sha1_hash;

    bool operator<(const Entry &other) const
    {
        return filename < other.filename;
    }
};

#define CHUNK 16384

std::string sha1_hex(const std::string &input);
std::string compute_sha1(const std::string &data, bool print_out);
std::string sha_file(std::string basicString);
std::string hash_digest(const std::string &input);
void compressFile(const std::string &data, uLong *bound, unsigned char *dest);
void compress_and_store(const std::string &hash, const std::string &content, std::string dir);
bool decompress_object(std::string &buf, const std::string &data);
bool compress_object(std::string &buf, const std::string &data);
int decompress(FILE *source, FILE *dest);
std::set<Entry> parse_tree_object(FILE *file);
fs::path locateGitFolderRelative(const fs::path &startDir);
fs::path locateGitFolder(const fs::path &startDir);
#endif
