// implements the git checkout command using the commit hash of previous commits
#ifndef GIT_CHECKOUT_H
#define GIT_CHECKOUT_H
#include <iostream>
#include <string>
#include "utils.h"

int git_checkout(int argc, char *argv[]);
void update_working_directory(string &tree_hash);
#endif