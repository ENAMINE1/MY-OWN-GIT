#include <iostream>
#include <fstream>
#include <string>
#include <zlib.h>
#include <filesystem> // Add this line
#include "cat_file.h"
#include "utils.h"

int git_cat_file(int argc, char *argv[])
{
    if (argc <= 3)
    {
        std::cerr << "Invalid arguments, required `-p <blob_sha>`\n";
    }
    std::string flag = argv[2];
    // only implemented -p flag
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
    return 0;
}
