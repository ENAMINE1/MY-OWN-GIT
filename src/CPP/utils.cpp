#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <zlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <fstream>
#include <filesystem>
#include "utils.h"

std::string sha1_hex(const std::string &filepath)
{
    // Read the file content
    std::cerr << "Reading file " << filepath << std::endl;
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

std::string compute_sha1(const std::string &data, bool print_out = false)
{
    unsigned char hash[20]; // 160 bits long for SHA1
    SHA1(reinterpret_cast<const unsigned char *>(data.c_str()), data.size(), hash);
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (const auto &byte : hash)
    {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string sha_file(std::string data)
{
    unsigned char hash[20];
    SHA1((unsigned char *)data.c_str(), data.size(), hash);
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (const auto &byte : hash)
    {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    std::cout << ss.str() << std::endl;
    return ss.str();
}
// this function is used to convert the hexadecimal hash to a binary hash which means that the hash is converted to a string of bytes
std::string hash_digest(const std::string &input)
{
    std::string condensed;
    for (size_t i = 0; i < input.length(); i += 2)
    {
        std::string byte_string = input.substr(i, 2);
        char byte = static_cast<char>(std::stoi(byte_string, nullptr, 16));
        condensed.push_back(byte);
    }
    return condensed;
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

/*
    example of entry of a tree object
    "tree"+bytes+'\0'+entry1+'\0'+entry2+'\0'+entry3
    where each entry is of the form
    100644 file.txt\0<40-byte SHA-1 hash for file.txt> "OR"
    040000 dir\0<20-byte SHA-1 hash for the tree object "dir">
*/

std::set<Entry> read_entries(FILE *file)
{
    std::set<Entry> entries;
    char buffer[6 + 1]; // Buffer to read the mode (6 characters + null terminator)
    char space;         // To read the space character

    while (fread(buffer, 1, 6, file) == 6 && fread(&space, 1, 1, file) == 1)
    {
        if (space != ' ')
        {
            std::cerr << "Error: Expected space after mode!" << std::endl;
            break;
        }

        Entry entry;

        // Read mode (6 characters)
        buffer[6] = '\0'; // Null terminate the buffer to make it a C-string
        entry.mode = buffer;

        // Read filename until the null terminator
        char ch;
        entry.filename.clear();
        while (fread(&ch, 1, 1, file) == 1 && ch != '\0')
        {
            entry.filename += ch;
        }

        if (ch != '\0')
        {
            std::cerr << "Error: Null terminator not found after filename!" << std::endl;
            break;
        }

        // Read SHA-1 hash (20 bytes)
        char hash[41];
        if (fread(hash, 1, 40, file) != 40)
        {
            std::cerr << "Error: Could not read 20-byte SHA-1 hash!" << std::endl;
            break;
        }
        hash[40] = '\0'; // Null terminate the buffer to make it a C-string
        entry.sha1_hash.assign(hash, 41);

        entries.insert(entry);
    }

    return entries;
}

std::set<Entry> parse_tree_object(FILE *tree_object)
{
    rewind(tree_object); // set the file position indicator to the beginning of the file
    // print the tree_object file content
    char ch;
    bool foundNull = false;
    std::string object_type;
    // Read the file until the first '\0' character
    while (fread(&ch, 1, 1, tree_object) == 1)
    {
        object_type.push_back(ch);
        if (ch == '\0')
        {
            foundNull = true;
            break;
        }
    }
    if(!foundNull)
    {
        std::cerr<<"Fatal: Invalid tree object format."<<std::endl;
        return {};
    }
    else if(object_type.substr(0,4) != "tree")
    {
        std::cerr<<"Fatal: Invalid object type."<<std::endl;
        return {};
    }
    std::set<Entry> sorted_directories = read_entries(tree_object);
    return sorted_directories;
}

bool decompress_object(std::string &buf, std::string data)
{
    buf.resize(data.size());
    while (true)
    {
        auto buf_size = buf.size();
        auto res = uncompress(reinterpret_cast<Bytef *>(buf.data()), &buf_size, reinterpret_cast<const Bytef *>(data.data()), data.size());
        if (res == Z_BUF_ERROR)
        {
            buf.resize(buf.size() * 2);
        }
        else if (res != Z_OK)
        {
            std::cerr << "Failed to decompress file, code:" << res << "\n";
            return false;
        }
        else
        {
            buf.resize(buf_size);
            break;
        }
    }
    return true;
}

bool compress_object(std::string &buf, std::string data)
{
    buf.resize(data.size() + data.size() / 100 + 12);
    while (true)
    {
        auto buf_size = buf.size();
        auto res = compress(reinterpret_cast<Bytef *>(buf.data()), &buf_size, reinterpret_cast<const unsigned char *>(data.c_str()), data.size());
        if (res == Z_BUF_ERROR)
        {
            buf.resize(buf.size() * 2);
        }
        else if (res != Z_OK)
        {
            std::cerr << "Failed to compress file, code:" << res << "\n";
            return false;
        }
        else
        {
            buf.resize(buf_size);
            break;
        }
    }
    return true;
}

void compress_and_store(const std::string &hash, const std::string &content, std::string dir = ".")
{
    // Open input stream to read from memory (fmemopen is POSIX, not standard C++)
    FILE *input = fmemopen((void *)content.c_str(), content.length(), "rb");
    if (!input)
    {
        std::cerr << "Failed to open memory stream for reading.\n";
        return;
    }

    std::string hash_folder = hash.substr(0, 2);
    std::string object_path = dir + "/.git/objects/" + hash_folder + '/';
    if (!std::filesystem::exists(object_path))
    {
        std::filesystem::create_directories(object_path);
    }

    std::string object_file_path = object_path + hash.substr(2);
    if (!std::filesystem::exists(object_file_path))
    {
        FILE *output = fopen(object_file_path.c_str(), "wb");
        if (!output)
        {
            std::cerr << "Failed to open output file for writing.\n";
            fclose(input); // close input file stream before returning
            return;
        }

        // Determine size of content to be compressed
        fseek(input, 0, SEEK_END);
        size_t input_size = ftell(input);
        fseek(input, 0, SEEK_SET);

        // Allocate buffer for input data
        Bytef *input_buffer = new Bytef[input_size];
        fread(input_buffer, 1, input_size, input);

        // Call compression function
        if (compress(input_buffer, &input_size, (const Bytef *)content.c_str(), content.length()) != Z_OK)
        {
            std::cerr << "Failed to compress data.\n";
            fclose(output);
            delete[] input_buffer;
            fclose(input); // close input file stream before returning
            return;
        }

        // Write compressed data to output file
        fwrite(input_buffer, 1, input_size, output);

        // Clean up
        delete[] input_buffer;
        fclose(output);
    }

    fclose(input);
}