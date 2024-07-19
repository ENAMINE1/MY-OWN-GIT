#include "git_init.h"

int git_init()
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

        std::cout << "Initialized an empty git directory.\n";
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return 0;
}
