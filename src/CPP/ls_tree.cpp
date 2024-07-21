#include "ls_tree.h"

// format of file object
/*
    example of entry of a tree object
    "tree"+bytes+'\0'+entry1+'\0'+entry2+'\0'+entry3 ....
    where each entry is of the form
    100644 file.txt\0<20-byte SHA-1 hash for file.txt> "OR"
    040000 dir\0<20-byte SHA-1 hash for the tree object "dir">
*/

int ls_tree(const char *object_hash)
{
    // retrieve the object path
    char object_path[64];
    // getting the object path form the object hash
    // std::cout<<"Object hash: "<<object_hash<<endl;
    snprintf(object_path, sizeof(object_path), "%s/objects/%.2s/%s", git_path.c_str(), object_hash, object_hash + 2);
    // std::cout<<"Object path: "<<object_path<<endl;
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
    // decompress the tree sha1 hash and store the content of the tree object in the output file
    if (decompress(object_file, output_file) != EXIT_SUCCESS)
    {
        std::cerr << "Failed to decompress object file.\n";
        return EXIT_FAILURE;
    }
    std::set<Entry> directories = parse_tree_object(output_file);
    // print the directories
    if (!directories.empty())
    {
        // std::cout << "Directories: " << endl;
        for (const Entry &directory : directories)
        {
            std::string object_type;
            if (directory.mode == "040000")
                object_type = "tree";
            else
                object_type = "blob";
            std::cout << GREEN << directory.filename << RESET << endl;
            std::cout << GREEN << directory.mode << " " << object_type << " " << directory.sha1_hash << '\t' << fs::path(directory.filename).filename().c_str() << RESET << endl;
        }
    }
    return EXIT_SUCCESS;
}
