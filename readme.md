# MY OWN GIT - Containerized Environment Setup 🚀

Welcome to the **MY OWN GIT** project! This project tries to mimic some of the features of Git. The primary goal is to appreciate the effort behind such a marvelous technology that we all use today and to understand how things work behind the scenes.

If you are intrigued by the workings of Git and want to learn about its internals, then this repo is for you. Hope you get to learn something new! ✨

The tool includes several commands such as `init`, `add`, `commit`, and `clone`.

**Here is an in depth guide on the internas of the .git folder - [What is in that .git directory?](https://blog.meain.io/2023/what-is-in-dot-git/) 🤯.**

## Table of Contents

1. [Prerequisites](#prerequisites)
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

## Prerequisites

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

## Building the Docker Image 🛠️

1. Clone the repository to your local machine.
   ```bash
   git clone https://github.com/yourusername/git_clone_tool.git
   cd git_clone_tool
   ```
2. Build the Docker image.
   ```bash
   docker build -t git_clone_tool:latest .
   ```

## Running the Docker Container 🏃‍♂️

1. Run the Docker container.
   ```bash
   docker run -it git_clone_tool:latest
   ```

This command will start a bash session inside the container, allowing you to interact with the `git_clone` tool.

## Using the Git Clone Tool 🛠️

Once inside the Docker container, you can use the `git_clone` tool with the following commands:

### Commands

- **init**: Initialize a new repository.
  ```bash
  git_clone init
  ```

- **add**: Add a file to the staging area.
  ```bash
  git_clone add <file>
  ```

- **commit**: Commit staged changes.
  ```bash
  git_clone commit -m "Commit message"
  ```

- **clone**: Clone an existing repository.
  ```bash
  git_clone clone <repository_url>
  ```

## File Structure 📂

```
.
├── CMakeCache.txt
├── cmake_install.cmake
├── CMakeLists.txt
├── Dockerfile
├── Makefile
├── readme.md
└── src
    ├── CPP
    │   ├── cat_file.cpp
    │   ├── git_add.cpp
    │   ├── git_init.cpp
    │   ├── hash_object.cpp
    │   ├── ls_tree.cpp
    │   ├── main.cpp
    │   ├── utils.cpp
    │   └── write_tree.cpp
    └── Headers
        ├── cat_file.h
        ├── git_add.h
        ├── git_init.h
        ├── hash_object.h
        ├── ls_tree.h
        ├── utils.h
        └── write_tree.h
```

## Contributing 🤝

If you would like to contribute to this project, please follow the steps below:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Make the appropriate changes in the files.
4. Add changes to reflect the changes made.
5. Commit your changes (`git commit -am 'Add new feature'`).
6. Push to the branch (`git push origin feature-branch`).
7. Create a Pull Request.

Thank you for your contribution! 🌟

---

Happy coding! 💻✨