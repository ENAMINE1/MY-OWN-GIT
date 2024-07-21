#include "git_checkout.h"

// usage git_2.0 checkout <commit-hash>
FILE *prev_commit(std::string &commit_hash)
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

void process_tree_object(const std::string &tree_hash, std::set<Entry> &file_entries)
{
    char object_path[64];
    snprintf(object_path, sizeof(object_path), ".git/objects/%.2s/%s", tree_hash.c_str(), tree_hash.c_str() + 2);

    FILE *object_file = fopen(object_path, "rb");
    if (object_file == nullptr)
    {
        std::cerr << "Invalid object hash: " << tree_hash << "\n";
        return;
    }

    FILE *output_file = tmpfile();
    if (output_file == nullptr)
    {
        std::cerr << "Failed to create output file.\n";
        fclose(object_file);
        return;
    }

    // Decompress the tree object into the output file
    if (decompress(object_file, output_file) != EXIT_SUCCESS)
    {
        std::cerr << "Failed to decompress object file.\n";
        fclose(object_file);
        fclose(output_file);
        return;
    }

    rewind(output_file);

    // Parse the tree object to extract directories and files
    std::set<Entry> directories = parse_tree_object(output_file);

    // Process each entry in the directories
    for (const Entry &entry : directories)
    {
        if (entry.mode == "040000") // If it's a tree object
        {
            // Recursively process the tree object
            process_tree_object(entry.sha1_hash, file_entries);
        }
        else // If it's a blob (file)
        {
            file_entries.insert(entry);
        }
    }

    fclose(object_file);
    fclose(output_file);
}

int git_checkout(int argc, char *argv[])
{
    if (argc > 3)
    {
        std::cout << "fatal | Correct usage: git_2.0 checkout <commit-hash>" << '\n';
        return 1;
    }
    else
    {
        std::string commit_hash = argv[2];
        FILE *object_file = prev_commit(commit_hash);
        // now i have to decompress the object file
        FILE *output_file = tmpfile();
        decompress(object_file, output_file);

        rewind(output_file);

        // using this tree object to make changes to the files in the working directory
        fs::path root_dir = locateParentFolderRelative(curr_dir);
        std::string homedir = root_dir.string();
        std::string current_tree_hash = write_tree(homedir);
        std::cout << homedir << '\n';
        std::string prev_tree_hash;
        // Read the commit object from output_file
        std::string commit_content;
        char buffer[4096]; // Buffer for reading lines
        while (fgets(buffer, sizeof(buffer), output_file) != nullptr)
        {
            std::string temp = buffer;
            int idx = temp.find("tree");
            if (idx != std::string::npos)
            {
                prev_tree_hash = temp.substr(idx + 5, 40);
                break;
            }
            else
                commit_content += buffer;
        }
        std::cout << MAGENTA << "current tree hash: " << current_tree_hash << RESET << '\n';
        std::cout << MAGENTA << "prev tree hash: " << prev_tree_hash << RESET << '\n';

        fclose(object_file);
        fclose(output_file);

        // Use std::set<Entry> to store file entries
        std::set<Entry> file_entries;
        process_tree_object(prev_tree_hash, file_entries);
        std::set<Entry> curr_file_entries;
        process_tree_object(current_tree_hash, curr_file_entries);
        // Print the file entries
        if (!file_entries.empty())
        {
            std::cout << "Files:\n";
            for (const Entry &entry : file_entries)
            {
                std::cout << GREEN << entry.filename << RESET << '\n';
                std::cout << GREEN << entry.mode << " " << entry.sha1_hash << '\t' << fs::path(entry.filename).filename().c_str() << RESET << '\n';
            }
        }
        if (!curr_file_entries.empty())
        {
            std::cout << "Files:\n";
            for (const Entry &entry : curr_file_entries)
            {
                std::cout << MAGENTA << entry.filename << RESET << '\n';
                std::cout << MAGENTA << entry.mode << " " << entry.sha1_hash << '\t' << fs::path(entry.filename).filename().c_str() << RESET << '\n';
            }
        }
    }
    return 0;
}
