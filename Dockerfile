# Use a base image that includes the necessary environment (e.g., Ubuntu, Alpine, etc.)
FROM ubuntu:latest

# Set the working directory
WORKDIR /app

# Install dependencies
RUN apt-get update \
    && apt-get install -y \
    cmake \
    gcc \
    g++

RUN apt-get update \
    && apt-get install -y \
    zlib1g \
    zlib1g-dev \
    libssl-dev

RUN dpkg -s zlib1g libssl-dev

# Create necessary directories
RUN mkdir -p /app/src/CPP /app/src/Headers

# Copy source code and CMakeLists.txt
COPY ./src/CPP /app/src/CPP
COPY ./src/Headers /app/src/Headers
COPY ./CMakeLists.txt /app/CMakeLists.txt

# Build the project
RUN cd /app
RUN cmake .
RUN make

# Copy the built executable to a directory in the PATH
RUN cp /app/git_c /usr/local/bin/git_clone


# Set executable permissions
RUN chmod +x /usr/local/bin/git_clone

# Define the command that should be run when a container is started from this image
CMD ["bash"]
