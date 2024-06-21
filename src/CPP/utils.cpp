#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <zlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <fstream>
#include "utils.h"

std::string sha1_hex(const std::string &filepath)
{
    // Read the file content
    std::ifstream file(filepath, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    // Compute the SHA-1 hash
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char *>(content.c_str()), content.size(), hash);
    // Convert the hash to a hex string
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
    {
        ss << std::setw(2) << static_cast<int>(hash[i]);
    }
    return ss.str();
}

std::string hash_digest(const std::string &input)
{
    unsigned char hash[20];
    SHA1((unsigned char *)input.c_str(), input.size(), hash);
    return std::string((char *)hash, 20);
}

void compressFile(const std::string &data, uLong *bound, unsigned char *dest)
{
    compress(dest, bound, (const unsigned char *)data.c_str(), data.size());
}

int decompress(FILE *source, FILE *dest)
{
    // ret: Variable to hold the return codes from zlib functions.
    int ret;
    // have: Variable to keep track of how much data has been decompressed.
    unsigned have;
    // z_stream strm: Structure required by zlib to manage the decompression process.
    z_stream strm;
    // in[CHUNK] and out[CHUNK]: Buffers for input and output data.
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    // Initialize the z_stream structure.
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    // inflateInit initializes the z_stream structure for decompression.
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    do
    {
        // Read data from the source file into the input buffer.
        strm.avail_in = fread(in, 1, CHUNK, source);
        // if ferror(source) returns a non-zero value, an error occurred while reading from the source file.
        if (ferror(source))
        {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            switch (ret)
            {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest))
            {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

    } while (ret != Z_STREAM_END);

    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

std::set<std::string> parse_tree_object(FILE *tree_object)
{
    rewind(tree_object); // set the file position indicator to the beginning of the file

    std::vector<std::string> unsorted_directories;
    char mode[7];
    char type[5];
    char filename[256];
    unsigned char hash[20];
    while (fscanf(tree_object, "%6s", mode) != EOF)
    {
        // read the filename (up to the null byte)
        int i = 0;
        int c;
        while ((c = fgetc(tree_object)) != 0 && c != EOF)
        {
            // if the character is a blank space, continue
            if (c == ' ')
            {
                continue;
            }
            filename[i++] = c;
        }
        filename[i] = '\0'; // null-terminate the filename
        mode[6] = '\0';
        // read the hash
        fread(hash, 1, 20, tree_object);
        // shift the mode string to the right and prepend 0 till the number contains 6 digits


        std::string file_details = std::string(mode) + " " + std::string(reinterpret_cast<char *>(hash), 20) + " " + std::string(filename);
        unsorted_directories.push_back(file_details);
    }
    std::sort(unsorted_directories.begin(), unsorted_directories.end());                                // sort the directories lexicographically
    std::set<std::string> sorted_directories(unsorted_directories.begin(), unsorted_directories.end()); // remove duplicates
    return sorted_directories;
}
