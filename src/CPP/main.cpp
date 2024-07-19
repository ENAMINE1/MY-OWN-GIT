#include <iostream>
#include <string>
#include <fstream>
#include "git_init.h"
#include "hash_object.h"
#include "ls_tree.h"
#include "cat_file.h"
#include "write_tree.h"
#include "git_add.h"
#include "commit_tree.h"
#include "git_checkout.h"
#include "utils.h"

// argv[0] = "./git_c" arg[1] = command arg[2] = flags (optional) arg[3] = file
fs::path curr_dir;
fs::path git_dir;
string git_path;

int main(int argc, char *argv[])
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 2)
    {
        std::cerr << "No command provided.\n";
        return EXIT_FAILURE;
    }
    std::string command = argv[1];
    if (command != "init")
    {
        curr_dir = fs::current_path();
        git_dir = locateGitFolderRelative(curr_dir);
        git_path = git_dir.string();
        if (git_dir.empty())
        {
            cout << "Initialise a Git repository first!" << endl;
            return 0;
        }
    }

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
        if (argc < 4)
        {
            std::cerr << "Incorrect usage of hash-object.\nUsage hash-object -w <file_path>\n";
            exit(EXIT_FAILURE);
        }
        std::string filepath(argv[3]);
        std::string hash_value = hash_object(filepath);
        if (hash_value != "")
            std::cout << hash_value << '\n';
    }
    else if (command == "ls-tree")
    {
        if (argc < 3)
        {
            std::cerr << "Less than 3 arguments\n";
            return EXIT_FAILURE;
        }
        if (ls_tree(argv[2]))
        {
            std::cerr << "Less than 3 arguments\n";
            return EXIT_FAILURE;
        }
    }
    else if (command == "write-tree")
    {
        // usage git_clone wirte-tree directory path
        if (argc < 3)
            std::cout << "Usage: write-tree <file_path>" << endl;
        else if (argc > 3)
            std::cout << "Too many parmeters to write-tree.\n Usage: write-tree <file_path>" << endl;
        std::string tree_hash = write_tree(".");
        if (tree_hash.empty())
        {
            return EXIT_FAILURE;
        }
        std::cout << tree_hash << "\n";
    }
    else if (command == "add")
    {
        if (argc < 2)
        {
            std::cerr << "No file provided\n";
            return EXIT_FAILURE;
        }
        git_add(argv[2]);
    }
    else if (command == "commit-tree")
    {
        if (argc < 5)
        {
            std::cerr << "Fatal Error: Usage commit-tree <tree-hash> -m <commit-message>\n";
            return EXIT_FAILURE;
        }
        if (argc > 5)
        {
            // if no file with the name COMMIT_EDITMSG is present then wrong command because no earlier comimits
            string file_path = git_path + "/COMMIT_EDITMSG";
            // cout << MAGENTA << file_path << RESET << endl;
            std::ifstream file(file_path);
            if (!file)
                std::cerr << "Fatal Errro: No previous commits found\n";
            else
            {
                if (argc > 7)
                {
                    std::cerr << "Fatal Error: Usage commit-tree <tree-hash> -p <parent-hash> -m <commit-message>\n";
                    return EXIT_FAILURE;
                }
                else
                {
                    std::ofstream file(file_path);
                    if (!file)
                    {
                        std::cerr << "Fatal Error: Failed to create .git/COMMIT_EDITMSG file\n";
                        return EXIT_FAILURE;
                    }
                    file << argv[6];
                    file.close();
                    std::string commit_hash = commit_tree(argv[2], argv[6], argv[4]);
                    std::cout << commit_hash << '\n';
                    return EXIT_SUCCESS;
                }
            }
        }
        // create ./git/COMMIT_EDITMSG file and write the commit message to it
        std::ofstream file("file_path");
        if (!file)
        {
            std::cerr << "Fatal Error: Failed to create .git/COMMIT_EDITMSG file\n";
            return EXIT_FAILURE;
        }
        file << argv[4];
        file.close();
        std::string commit_hash = commit_tree(argv[2], argv[4]);
        std::cout << commit_hash << '\n';
        return EXIT_SUCCESS;
    }
    else if (command == "checkout")
    {
        if (argc < 3)
        {
            std::cerr << "Fatal Error: Usage git_2.0 checkout <commit-hash>\n";
            return EXIT_FAILURE;
        }
        git_checkout(argc, argv);
    }
    else if(command == "add")
    {
        if(argc < 3)
        {
            std::cerr << "No file or folder provided\n";
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
