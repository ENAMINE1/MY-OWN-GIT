#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <set>
#include <cstdio>
#include <zlib.h>

#define CHUNK 16384

std::string sha1_hex(const std::string &input);
std::string hash_digest(const std::string &input);
void compressFile(const std::string &data, uLong *bound, unsigned char *dest);
int decompress(FILE *source, FILE *dest);
std::set<std::string> parse_tree_object(FILE *file);

#endif
