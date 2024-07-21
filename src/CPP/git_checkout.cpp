#include "git_checkout.h"

// usage git_2.0 checkout <commit-hash>
FILE * prev_commit(std::string& commit_hash)
{
    char object_path[64];
    snprintf(object_path, sizeof(object_path), ".git/objects/%.2s/%s", commit_hash.c_str(), commit_hash.c_str() + 2);
    FILE *object_file = fopen(object_path, "rb");
    if (object_file == nullptr)
    {
        std::cerr << "Invalid object hash: " << commit_hash << "\n";
        exit(EXIT_FAILURE);
    }
    return object_file;
}

int git_checkout(int argc, char *argv[])
{
    if(argc > 3)
    {
        cout << "fatal | Correct usage: git_2.0 checkout <commit-hash>" << endl;
        return 1;
    }
    else
    {
        std::string commit_hash = argv[2];
        FILE *object_file = prev_commit(commit_hash);
        // now i have to decompress the object file
        FILE* output_file = tmpfile();
        decompress(object_file, output_file);

        // using this tree object to make changes to the files in the working directory

        rewind(output_file);
        fs::path root_dir = locateParentFolderRelative(curr_dir);
        string homedir = root_dir.string();
        string current_tree_hash = write_tree(homedir);
        string prev_tree_hash;
        // Read the commit object from output_file
        std::string commit_content;
        char buffer[4096]; // Buffer for reading lines
        while (fgets(buffer, sizeof(buffer), output_file) != nullptr)
        {
            // the first line will be of the form commit <length>'\0'tree <tree_hash> so we replace the '\0' with '\n'
            std::string temp = buffer;
            int idx = temp.find("tree");
            if(idx != string::npos)
            {
                prev_tree_hash = temp.substr(idx + 5, 40);
                break;
            }
            else
            commit_content += buffer;
        }
        cout <<GREEN<< "current tree hash: " << current_tree_hash << RESET << endl;
        cout <<GREEN<< "prev tree hash: " << prev_tree_hash << RESET << endl;

        fclose(object_file);
        fclose(output_file);

        // update_working_directory(tree_hash);
    }
    return 0;
}

void update_working_directory(std::string &tree_hash)
{

}