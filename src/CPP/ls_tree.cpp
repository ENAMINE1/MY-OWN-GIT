#include <iostream>
#include <filesystem>
#include <fstream>
#include<cstring>
#include <sstream>
#include <set>
#include <zlib.h>
#include "ls_tree.h"
#include "utils.h"

// format of file object


// format of the tree object
// <mode> SP <treehash>\0<filename>

/* For files, the valid values are:
    100644(regular file)
    100755(executable file)
    120000(symbolic link)
    For directories, the value is 040000*/

// std::set<std::string> parse_tree_object(FILE *file)
// {
//     std::set<std::string> directories;
//     char buffer[CHUNK];
//     while (fgets(buffer, sizeof(buffer), file) != NULL)
//     {
//         // parse the tree object
//         TreeEntry entry;
//         char *token = strtok(buffer, " ");
//         entry.mode = token;
//         token = strtok(NULL, " ");
//         entry.type = token;
//         token = strtok(NULL, " ");
//         entry.hash = token;
//         token = strtok(NULL, "\n");
//         entry.name = token;
//         // check if the entry is a directory
//         if (entry.type == "tree")
//         {
//             directories.insert(entry.name);
//         }
//     }
//     return directories;
// }
int ls_tree(const char *object_hash)
{
    // retrieve the object path
    char object_path[64];
    // getting the object path form the object hash
    // std::cout<<"Object hash: "<<object_hash<<std::endl;
    snprintf(object_path, sizeof(object_path), ".git/objects/%.2s/%s", object_hash, object_hash + 2);
    // std::cout<<"Object path: "<<object_path<<std::endl;
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
    std::cout << "Directories: " << std::endl;
    for (const std::string &directory : directories)
    {
        std::cout << directory << '\n';
    }
    return EXIT_SUCCESS;
}
