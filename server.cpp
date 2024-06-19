#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <zlib.h>
#include <sstream>
#include <openssl/sha.h>
#include <algorithm>
#include <numeric>
#include <set>
#include <ctime>
#include <cstring>
#define CHUNK 16384

std::string sha_file(std::string basicString);
void compressFile(const std::string data, uLong *bound, unsigned char *dest);
int ls_tree(const char *object_hash);
void hash_object(std::string file);
std::string hash_digest(const std::string &input);

int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    
    if (argc < 2)
    {
        std::cerr << "No command provided.\n";
        return EXIT_FAILURE;
    }

    std::string command = argv[1];
    // std::cout << command << std::endl;
    if (command == "init")
    {
        try
        {
            std::filesystem::create_directory(".git");
            std::filesystem::create_directory(".git/objects");
            std::filesystem::create_directory(".git/refs");

            std::ofstream headFile(".git/HEAD");
            if (headFile.is_open())
            {
                headFile << "ref: refs/heads/main\n";
                headFile.close();
            }
            else
            {
                std::cerr << "Failed to create .git/HEAD file.\n";
                return EXIT_FAILURE;
            }

            std::cout << "Initialized git directory\n";
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cerr << e.what() << '\n';
            return EXIT_FAILURE;
        }
    }
    else if (command == "cat-file")
    {
        if (argc <= 3)
        {
            std::cerr << "Invalid arguments, required `-p <blob_sha>`\n";
        }
        std::string flag = argv[2];
        if (flag != "-p")
        {
            std::cerr << "Invalid flag for cat-file, expected -p\n";
        }
        // now i am sure with the arg count and the flag
        // the third argument contains the actual file name
        const std::string value = argv[3];
        // the first two characters denote the subfolder name inside the objects folder
        const std::string dir_name = value.substr(0, 2);
        // the rest of the sha1 hash value is the actual file name inside the folder
        const std::string blob_sha = value.substr(2);
        const auto blob_path = std::filesystem::path(".git") / "objects" / dir_name / blob_sha;

        std::ifstream input = std::ifstream(blob_path);
        if (!input.is_open())
        {
            std::cerr << "Failed to open " << blob_path << " file.\n";
            return EXIT_FAILURE;
        }

        const std::string blob_data = std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
        // std::cout << blob_data;
        // i have the compressed data of the file which needs to be decompressed using zlib
        auto buf = std::string();
        buf.resize(blob_data.size());
        while (true)
        {
            auto len = buf.size();
            if (auto res = uncompress((uint8_t *)buf.data(), &len, (const uint8_t *)blob_data.data(), blob_data.size()); res == Z_BUF_ERROR)
            {
                buf.resize(buf.size() * 2);
            }
            else if (res != Z_OK)
            {
                std::cerr << "Failed to uncompress Zlib. (code: " << res << ")\n";
                return EXIT_FAILURE;
            }
            else
            {
                buf.resize(len);
                break;
            }
        }
        std::cout << std::string_view(buf).substr(buf.find('\0') + 1);
    }
    else if (command == "hash-object")
    {
        std::string filepath(argv[3]);
        hash_object(filepath);
    }
    else if (command == "ls-tree")
    {
        if (argc < 3)
        {
            std::cerr << "Less than 3 argument " << '\n';
            return EXIT_FAILURE;
        }
        if (ls_tree(argv[3]))
        {
            std::cerr << "Less than 3 argument " << '\n';
            return EXIT_FAILURE;
        }
    }
    else
    {
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void hash_object(std::string file)
{
    // Read file contents
    std::ifstream t(file);
    std::stringstream data;
    data << t.rdbuf();
    // Create blob content string
    std::string content = "blob " + std::to_string(data.str().length()) + '\0' + data.str();
    // Calculate SHA1 hash
    std::string buffer = sha_file(content);
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
void compressFile(const std::string data, uLong *bound, unsigned char *dest)
{
    compress(dest, bound, (const Bytef *)data.c_str(), data.size());
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

/**************************************************************Tree decompression*************************************************************/

int decompress(FILE *input, FILE *output)
{
    // initialize decompression stream
    // std::cout << "Initializing decompression stream.\n";
    z_stream stream = {0};
    if (inflateInit(&stream) != Z_OK)
    {
        std::cerr << "Failed to initialize decompression stream.\n";
        return EXIT_FAILURE;
    }
    // initialize decompression variables
    char in[CHUNK];
    char out[CHUNK];
    bool haveHeader = false;
    char header[64];
    int ret;
    do
    {
        stream.avail_in = fread(in, 1, CHUNK, input);           // read from input file
        stream.next_in = reinterpret_cast<unsigned char *>(in); // set input stream
        if (ferror(input))
        {
            std::cerr << "Failed to read from input file.\n";
            return EXIT_FAILURE;
        }
        if (stream.avail_in == 0)
        {
            break;
        }
        do
        {
            stream.avail_out = CHUNK;                                 // set output buffer size
            stream.next_out = reinterpret_cast<unsigned char *>(out); // set output stream
            ret = inflate(&stream, Z_NO_FLUSH);                       // decompress
            if (ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
            {
                std::cerr << "Failed to decompress file.\n";
                return EXIT_FAILURE;
            }
            // write header to output file
            unsigned headerLen = 0, dataLen = 0;
            if (!haveHeader)
            {
                sscanf(out, "%s %u", header, &dataLen);
                haveHeader = true;
                headerLen = strlen(out) + 1;
            }
            // write decompressed data to output file
            if (dataLen > 0)
            {
                if (fwrite(out + headerLen, 1, dataLen, output) != dataLen)
                {
                    std::cerr << "Failed to write to output file.\n";
                    return EXIT_FAILURE;
                }
            }
        } while (stream.avail_out == 0);

    } while (ret != Z_STREAM_END);
    return inflateEnd(&stream) == Z_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}

std::set<std::string> parse_tree_object(FILE *tree_object)
{
    rewind(tree_object); // set the file position indicator to the beginning of the file

    std::vector<std::string> unsorted_directories;
    char mode[7];
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
        // read the hash
        fread(hash, 1, 20, tree_object);
        unsorted_directories.push_back(filename);
    }
    std::sort(unsorted_directories.begin(), unsorted_directories.end());                                // sort the directories lexicographically
    std::set<std::string> sorted_directories(unsorted_directories.begin(), unsorted_directories.end()); // remove duplicates
    return sorted_directories;
}

int ls_tree(const char *object_hash)
{
    // retrieve the object path
    char object_path[64];
    snprintf(object_path, sizeof(object_path), ".git/objects/%.2s/%s", object_hash, object_hash + 2);
    // set the input and output file descriptors
    FILE *object_file = fopen(object_path, "rb");
    if (object_file == NULL)
    {
        std::cerr << "Invalid object hash.\n";
        return EXIT_FAILURE;
    }
    FILE *output_file = tmpfile();
    if (output_file == NULL)
    {
        std::cerr << "Failed to create output file.\n";
        return EXIT_FAILURE;
    }
    if (decompress(object_file, output_file) != EXIT_SUCCESS)
    {
        std::cerr << "Failed to decompress object file.\n";
        return EXIT_FAILURE;
    }
    std::set<std::string> directories = parse_tree_object(output_file);
    // print the directories
    for (const std::string &directory : directories)
    {
        std::cout << directory << '\n';
    }
    return EXIT_SUCCESS;
}
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