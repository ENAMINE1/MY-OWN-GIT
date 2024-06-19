#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <set>
#include <zlib.h>
#include "ls_tree.h"
#include "utils.h"

int ls_tree(const char *object_hash)
{
    // retrieve the object path
    char object_path[64];
    // format the object path
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
