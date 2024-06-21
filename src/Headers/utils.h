#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <set>
#include <cstdio>
#include <zlib.h>

// example of a tree object:
// 100644 blob 3cf56579491f151d82b384c211cf1971c300fbf8 .dockerignore
// 040000 tree 918756f1a4e5d648ae273801359c440c951555f9    build
struct TreeEntry
{
    std::string mode;
    std::string type;
    std::string hash;
    std::string name;
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
std::set<std::string> parse_tree_object(FILE *file);

#endif
