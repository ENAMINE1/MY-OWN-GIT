#!/bin/bash

# Run the git_2.0 executable with the init command
git_2.0 init

# Create a directory named "test_repo"
echo "File in root directory" > file0.txt
mkdir test_repo
cd test_repo

# Create some files and write content to them
echo "This is file1" > file1.txt
echo "This is file2" > file2.txt
echo "This is file3" > file3.txt

cd ..
# create the tree object

# Make a commit
# git_2.0 commit-tree $(git_2.0 write-tree .) -m "Initial commit"
git_2.0 write-tree .

# Modify one of the files and make another commit
cd test_repo
echo "This is an update to file1" >> file1.txt
git_2.0 write-tree .

git_2.0 commit-tree $(git_2.0 write-tree .) -m "Updated file1"


# Modify another file and make a third commit
echo "This is an update to file2" >> file2.txt
git_2.0 commit-tree $(git_2.0 write-tree .) -m "Updated file2"

# Now you have a repository with multiple commits to test the checkout functionality
echo "Repository setup complete. You can now test other commands."
