#include <sstream>
#include <zlib.h>
#include <openssl/sha.h>
#include <fstream>
#include <string>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <cstring>
#include "commit_tree.h"
#include "hash_object.h"
#include "utils.h"

std::string get_author()
{
    // std::string homeDir = getHomeDirectory();
    // std::string gitConfigPath = homeDir + "/.gitconfig";
    // std::ifstream configFile(gitConfigPath);

    // if (!configFile.is_open())
    // {
    //     std::cerr << "Failed to open Git config file." << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    // std::string line;
    // std::string userName;
    // std::string userEmail;
    // while (std::getline(configFile, line))
    // {
    //     if (line.find("name =") != std::string::npos)
    //     {
    //         userName = line.substr(line.find("=") + 2);
    //     }
    //     else if (line.find("email =") != std::string::npos)
    //     {
    //         userEmail = line.substr(line.find("=") + 2);
    //     }
    // }
    // if(userName.empty() && userEmail.empty())
    // {
    //     std::cerr << "Failed to get author information.\nSet author details using the command git_2.0 config --global user .\n";
    //     exit(EXIT_FAILURE);
    // }
    // else if(userName.empty())
    // {
    //     std::cerr << "Failed to get author name.\nSet author name using the command git_2.0 config --global user.name <username>.\n";
    //     exit(EXIT_FAILURE);
    // }
    // else if(userEmail.empty())
    // {
    //     std::cerr << "Failed to get author email.\nSet author email using the command git_2.0 config --global user.email <email>.\n";
    //     exit(EXIT_FAILURE);
    // }
    // configFile.close();
    // std::string author = userName + " < ";
    // author += userEmail;
    std::string author = " < No One Right Now> ";
    return author;
}

std::string
commit_tree(const std::string &tree_hash, const std::string &message)
{
    // create a commit object
    std::string commit_object = "tree " + tree_hash + '\n';
    commit_object += "author " + get_author() + '\n';
    commit_object += "committer " + get_author() + '\n';
    commit_object += '\n';
    commit_object += message + '\n';
    // create a blob object

    std::string commit_sha1 = hash_commit(commit_object);
    return commit_sha1;
}

std::string commit_tree(const std::string &tree_hash, const std::string &message, const std::string &parent_commit_hash)
{
    // create a commit object
    std::string commit_object = "tree " + tree_hash + '\n';
    commit_object += "parent " + parent_commit_hash + '\n';
    commit_object += "author " + get_author() + '\n';
    commit_object += "committer " + get_author() + '\n';
    commit_object += '\n';
    commit_object += message + '\n';
    // create a commit object
    std::string commit_sha1 = hash_commit(commit_object);
    return commit_sha1;
}

std::string hash_commit(const std::string &commit_content_str)
{
    int content_length = commit_content_str.size();
    std::ostringstream full_content;
    full_content << "commit " << content_length << "\0" << commit_content_str;
    std::string full_content_str = full_content.str();

    // calculate the sha1 hash of the commit object
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char *>(full_content_str.c_str()), full_content_str.size(), hash);

    // convert the hash to a string
    std::ostringstream hash_str;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        hash_str << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    compress_and_store(hash_str.str(), full_content_str, ".");
    return hash_str.str();
}

std::string getHomeDirectory()
{
    const char *homedir;
    if ((homedir = std::getenv("HOME")) == nullptr)
    {
        homedir = std::getenv("USERPROFILE");
    }
    return std::string(homedir);
}

std::string show_commit(const std::string &commit_hash)
{
    char object_path[64];
    snprintf(object_path, sizeof(object_path), ".git/objects/%.2s/%s", commit_hash.c_str(), commit_hash.c_str() + 2);
    FILE *object_file = fopen(object_path, "rb");
    if (object_file == nullptr)
    {
        std::cerr << "Invalid object hash: " << commit_hash << "\n";
        exit(EXIT_FAILURE);
    }

    FILE *output_file = tmpfile();
    if (output_file == nullptr)
    {
        std::cerr << "Failed to create output file.\n";
        exit(EXIT_FAILURE);
    }

    // Assuming decompress function works correctly
    if (decompress(object_file, output_file) != EXIT_SUCCESS)
    {
        std::cerr << "Failed to decompress object file.\n";
        exit(EXIT_FAILURE);
    }

    // Rewind the output file to beginning for reading
    rewind(output_file);

    // Read the commit object from output_file
    std::string commit_content;
    char buffer[4096]; // Buffer for reading lines
    while (fgets(buffer, sizeof(buffer), output_file) != nullptr)
    {
        // the first line will be of the form commit <length>'\0'tree <tree_hash> so we replace the '\0' with '\n'
        
        commit_content += buffer;
    }

    // Close the files
    fclose(object_file);
    fclose(output_file);

    // Print the commit content to stdout
    std::cout << commit_content;

    return commit_content;
}