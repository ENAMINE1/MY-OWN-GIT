# Use a base image that includes the necessary environment (e.g., Ubuntu, Alpine, etc.)
FROM ubuntu:latest

# Set the working directory
WORKDIR /app

# Install dependencies
RUN apt-get update \
    && apt-get install -y \
    cmake \
    gcc \
    g++ \
    zlib1g \
    zlib1g-dev \
    libssl-dev \
    tree \
    bash-completion

# Create a non-root user and group
RUN groupadd -r user && useradd -r -g user -m -d /home/user -s /bin/bash user

# Create necessary directories and set permissions
RUN mkdir -p /app/src/CPP /app/src/Headers \
    && chown -R user:user /app

# Copy source code and CMakeLists.txt
COPY ./src/CPP /app/src/CPP
COPY ./src/Headers /app/src/Headers
COPY ./CMakeLists.txt /app/CMakeLists.txt
COPY ./init.sh /home/user/init.sh

# Build the project
RUN cd /app \
    && cmake . \
    && make \
    && cp /app/git_c /usr/local/bin/git_2.0 \
    && chmod +x /usr/local/bin/git_2.0 \
    && chmod +x /home/user/init.sh

# Copy the Welcome.txt file to the user's home directory as .bashrc
COPY ./Welcome.txt /home/user/.bashrc

# Create directory for bash completion script and copy the script
RUN mkdir -p /etc/bash_completion.d
COPY ./git_clone_completion.sh /etc/bash_completion.d/git_clone_completion.sh
RUN chmod +r /etc/bash_completion.d/git_clone_completion.sh

RUN echo "source /etc/bash_completion.d/git_clone_completion.sh" >> /home/user/.bashrc

# Change ownership of the /app directory and /home/user directory to the non-root user
RUN chown -R user:user /app /home/user

# Switch to the non-root user
USER user

# Set the working directory to the non-root user's home directory
WORKDIR /home/user

# Define the command that should be run when a container is started from this image
CMD ["bash"]
