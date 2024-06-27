# MY OWN GIT - Containerized Environment Setup 🚀
---
Welcome to the **MY OWN GIT** project! This project tries to mimic some of the features of Git. The primary goal is to appreciate the effort behind such a marvelous technology that we all use today and to understand how things work behind the scenes.

If you are intrigued by the workings of Git and want to learn about its internals, then this repo is for you. Hope you get to learn something new! ✨

The tool includes several commands such as `init`, `add`, `commit`, and `clone`.

**Here is an in depth guide on the internals of the .git folder - [What is in that .git directory?](https://blog.meain.io/2023/what-is-in-dot-git/) 🤯.**
It is recommended to go through the above blog to get a feel for the interal workings of the .git folder.

## Table of Contents 📑

1. [System Requirements](#system-requirements)
2. [Installing Docker Desktop](#installing-docker-desktop)
   - [Windows](#windows)
   - [MacOS](#macos)
   - [Linux](#linux)
3. [Building the Docker Image](#building-the-docker-image)
4. [Running the Docker Container](#running-the-docker-container)
5. [Using the Git Clone Tool](#using-the-git-clone-tool)
   - [Commands](#commands)
6. [File Structure](#file-structure)
7. [Contributing](#contributing)

## System Requirements  📋

Ensure you have the following before starting:
- A machine with at least 2GB of RAM.
- Administrative access to install software.

## Installing Docker Desktop 🐳

### Windows

1. Download Docker Desktop for Windows from the [official Docker website](https://www.docker.com/products/docker-desktop).
2. Run the installer and follow the installation instructions.
3. After installation, Docker Desktop will start automatically. If not, you can start it from the Start menu.

### MacOS

1. Download Docker Desktop for Mac from the [official Docker website](https://www.docker.com/products/docker-desktop).
2. Open the `.dmg` file and drag Docker to the Applications folder.
3. Open Docker from the Applications folder.

### Linux

1. Docker Desktop is now available for Linux. Download Docker Desktop for Linux from the [official Docker website](https://www.docker.com/products/docker-desktop).
2. Follow the installation instructions provided on the website for your specific Linux distribution.

Here's how you can update the README for your "MY OWN GIT" project:

---

## Building the Docker Image 🛠️

To get started with the `git_2.0` tool, you can pull the Docker image directly from Docker Hub:

```bash
docker pull enamine/git_2.0
```

This command retrieves the latest version of the `git_2.0` Docker image, ready to use.


## Running the Docker Container 🏃‍♂️

1. Run the Docker container.
   ```bash
   docker run -it git_2.0:latest
   ```

This command will start a bash session inside the container, allowing you to interact with the `git_2.0` tool.

## Using the Git Clone Tool 🛠️

Once inside the Docker container, you can use the `git_2.0` tool with the following commands:

### Commands 📜

- **init**: Initialize a new repository.
  ```bash
  git_2.0 init
  ```

- **hash-object**: Compute the object ID and optionally creates a blob from a file.
   ```bash
   git_2.0 hash-object -w <file>
   ```

- **write-tree**: Write the current index to the repository.
   ```bash

   git_2.0 write-tree <filepath>
   ```

- **ls-tree**: List the contents of a tree object.
   ```bash
   git_2.0 ls-tree <tree_id>
   ```

- **cat-file**: Provide content or type and size information for repository objects.
   ```bash
   git_2.0 cat-file -p <object_id>
   ```

- **add**: Add a file to the staging area.
  ```bash
  git_2.0 add <file>
  ```

- **commit**: Commit staged changes.
  ```bash
  git_2.0 commit -m "Commit message"
  ```

- **clone**: Clone an existing repository.
  ```bash
  git_2.0 clone <repository_url>
  ```

## File Structure 📁

```
.
├── CMakeCache.txt
├── cmake_install.cmake
├── CMakeLists.txt
├── Dockerfile
├── git_clone_completion.sh
├── LICENSE.md
├── Makefile
├── readme.md
├── src
│   ├── CPP
│   │   ├── cat_file.cpp
│   │   ├── commit_tree.cpp
│   │   ├── git_add.cpp
│   │   ├── git_init.cpp
│   │   ├── hash_object.cpp
│   │   ├── ls_tree.cpp
│   │   ├── main.cpp
│   │   ├── utils.cpp
│   │   └── write_tree.cpp
│   └── Headers
│       ├── cat_file.h
│       ├── commit_tree.h
│       ├── git_add.h
│       ├── git_init.h
│       ├── hash_object.h
│       ├── ls_tree.h
│       └── write_tree.h
│       ├── utils.h
│
└── Welcome.txt
```

## Contributing 🌟

To contribute to this project and set up the development environment, follow these steps:

1. **Clone the repository**:
   ```bash
   git clone https://github.com/ENAMINE1/MY-OWN-GIT.git
   cd MY-OWN-GIT
   ```

2. **Build the Docker image**:
   ```bash
   docker build -t git_2.0:latest .
   ```

   Building the Docker image locally allows you to make changes and test them before committing.

3. **Run the Docker container**:
   ```bash
   docker run -it -v "$(pwd):/home/user/" git_2.0:latest
   ```

   This command starts a bash session inside the container, enabling you to interact with the `git_2.0` tool and test your changes.

4. **Make your changes**:
   - Modify the code or add new features as needed.

5. **Commit your changes**:
   ```bash
   git commit -am 'Add new feature'
   ```

6. **Push your changes**:
   ```bash
   git push origin feature-branch
   ```

7. **Create a Pull Request**:
   - Submit a Pull Request to merge your changes into the main branch of the repository.

Thank you for contributing to "MY OWN GIT"! 🌟



### Happy coding! 💻✨
---