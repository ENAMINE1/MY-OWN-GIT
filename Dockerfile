# Use a base image that includes the necessary environment (e.g., Ubuntu, Alpine, etc.)
FROM ubuntu:latest

# Install any dependencies your executable might need (if applicable)
# Example: Install git if your executable relies on Git commands
RUN apt-get update && apt-get install -y git


# Copy your custom executable into the container
COPY ./build/git_c /usr/local/bin/git_clone

# Set the executable bit (if not already set)
RUN chmod +x /usr/local/bin/git_clone

# Define the command that should be run when a container is started from this image
CMD ["bash"]
