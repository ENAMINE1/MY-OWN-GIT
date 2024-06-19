#include <iostream>
#include <string>
#include "git_init.h"
#include "hash_object.h"
#include "ls_tree.h"
#include "cat_file.h"
#include "write_tree.h"
#include "git_add.h"

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
    if (command == "init")
    {
        git_init();
    }
    else if (command == "cat-file")
    {
        git_cat_file(argc, argv);
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
            std::cerr << "Less than 3 arguments\n";
            return EXIT_FAILURE;
        }
        if (ls_tree(argv[3]))
        {
            std::cerr << "Less than 3 arguments\n";
            return EXIT_FAILURE;
        }
    }
    else if (command == "write-tree")
    {
        write_tree();
    }
    else if(command == "add")
    {
        if(argc < 2)
        {
            std::cerr << "No file provided\n";
            return EXIT_FAILURE;
        }
        git_add(argv[2]);
    }
    else
    {
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
