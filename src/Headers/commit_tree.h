#ifndef COMMIT_TREE_H
#define COMMIT_TREE_H

/*
commit object example:

tree d8329fc1cc938780ffdd9f94e0d364e0ea74f579
author Scott Chacon <schacon@gmail.com> 1243040974 -0700
committer Scott Chacon <schacon@gmail.com> 1243040974 -0700

First commit
*/

#include <string>
std::string commit_tree(const std::string &tree_hash, const std::string &message);
std::string commit_tree(const std::string &tree_hash, const std::string &message, const std::string &parent_commit_hash);
std::string get_author();
std::string hash_commit(const std::string &commit_object);
std::string getHomeDirectory();
std::string show_commit(const std::string &commit_hash);
#endif // COMMIT_TREE_H