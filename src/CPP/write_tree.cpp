#include "write_tree.h"

// The git write - tree command creates a tree object from the current state of the "staging area".The staging area is a place where changes go when you run git add. The staging area is also known as the "index".The tree object represents the state of the working directory at a particular point in time. It contains the names of files and directories, along with their modes and hashes.

// A tree object will contain the state of working directory in the form of other tree and blob objects
string write_tree(const std::string &directory)
{
    // std::cout << "Writing tree for directory: " << directory << std::endl;
    std::vector<std::string> tree_entries;
    std::vector<std::string> skip;
    skip.push_back(".git");
    std::regex hidden_file_regex(R"(^\..*)");

    // = {
    //     ".git", "server", "CMakeCache.txt",
    //     "CMakeFiles", "Makefile", "cmake_install.cmake"};
    // read the .gitignore file and store the entries in the skip vector
    fs::path start_dir = fs::current_path();
    string git_path = locateGitFolderRelative(start_dir);
    cout << YELLOW << __LINE__ << " write_tree.cpp " << " " << git_path << RESET << endl;
    std::ifstream gitignore(git_path + "/.gitignore");
    if (gitignore.is_open())
    {
        std::string line;
        while (std::getline(gitignore, line))
        {
            skip.push_back(line);
        }
    }

    for (const auto &entry : std::filesystem::directory_iterator(directory))
    {
        std::string path = entry.path().string();
        cout << BLUE << __LINE__ << " write_tree.cpp " << path << RESET << endl;
        if (std::any_of(skip.begin(), skip.end(), [&path](const std::string &s)
                        { return path.find(s) != std::string::npos; }))
        {
            continue;
        }
        // find the path to this file or dicretory relative to the directory where the git repository is located
        fs::path gitDir = locateGitFolder(start_dir);
        string git_path = gitDir.string();
        cout << GREEN << __LINE__ << " write_tree.cpp " << git_path << RESET << endl;
        // ignore hidden files
        if (std::regex_match(entry.path().filename().string(), hidden_file_regex))
        {
            continue;
        }
        std::string entry_type;
        std::error_code ec;
        if (std::filesystem::is_directory(entry.status()))
        {
            entry_type = "040000 "; // Directory
        }
        else
        {
            // Determine file permissions
            auto perms = std::filesystem::status(entry).permissions();
            if ((perms & std::filesystem::perms::owner_exec) != std::filesystem::perms::none ||
                (perms & std::filesystem::perms::group_exec) != std::filesystem::perms::none ||
                (perms & std::filesystem::perms::others_exec) != std::filesystem::perms::none)
            {
                entry_type = "100755 "; // Executable
            }
            else
            {
                entry_type = "100644 "; // Regular file
            }
        }
        std::string relative_path = path.substr(path.find(directory) + directory.length() + 1);
        std::string hash = std::filesystem::is_directory(path, ec) ? write_tree(path.c_str()) : hash_object(path.c_str());
        std::string object_type;
        if (entry_type == "040000 ")
            object_type = "tree";
        else
            object_type = "blob";
        std::cerr << entry_type + ' ' << object_type << ' ' + hash + '\t' + relative_path << std::endl;

        tree_entries.emplace_back(path + '\0' + entry_type + relative_path + '\0' + hash);
    }
    // sort the entries based on the absolute path O(nlogn)
    std::sort(tree_entries.begin(), tree_entries.end());
    // delete the path from the beginning of each entry
    int bytes = 0;
    /*
    example of entry of a tree object
    "tree"+bytes+'\0'+entry1+'\0'+entry2+'\0'+entry3
    where each entry is of the form
    100644 file.txt\0<20-byte SHA-1 hash for file.txt> "OR"
    040000 dir\0<20-byte SHA-1 hash for the tree object "dir">
    */
    for (auto &entry : tree_entries)
    {
        entry = entry.substr(entry.find('\0') + 1);
        bytes += entry.length();
    }
    // concatenate the entries
    std::string tree_content = "tree " + std::to_string(bytes) + '\0';
    for (const auto &entry : tree_entries)
    {
        tree_content += entry;
    }
    // std::cerr << tree_content << std::endl;
    // storing the tree object
    std::string tree_hash = compute_sha1(tree_content, false);
    cout << YELLOW << __LINE__ << " write_tree.cpp " << " " << tree_hash << RESET << endl;
    compress_and_store(tree_hash.c_str(), tree_content, git_path);
    // std::cout << "Tree hash for :" << directory << tree_hash << std::endl;
    return tree_hash;
}

std::string check_file_perms(std::filesystem::perms file_perms)
{
    using std::filesystem::perms;
    std::string ret = "100000 ";

    if ((perms::none != (file_perms & perms::owner_exec)))
        ret[3] += 1;
    if ((perms::none != (file_perms & perms::group_exec)))
        ret[4] += 1;
    if ((perms::none != (file_perms & perms::others_exec)))
        ret[5] += 1;
    if ((perms::none != (file_perms & perms::owner_write)))
        ret[3] += 2;
    if ((perms::none != (file_perms & perms::group_write)))
        ret[4] += 2;
    if ((perms::none != (file_perms & perms::others_write)))
        ret[5] += 2;
    if ((perms::none != (file_perms & perms::owner_read)))
        ret[3] += 4;
    if ((perms::none != (file_perms & perms::group_read)))
        ret[4] += 4;
    if ((perms::none != (file_perms & perms::others_read)))
        ret[5] += 4;
    return ret;
}
