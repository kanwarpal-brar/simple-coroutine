# Use a lightweight Linux image with amd64 architecture
FROM alpine:latest

# Install necessary tools and libraries
RUN apk update && apk add --no-cache \
    build-base \
    boost-dev \
    git \
    openssh \
    gdb \
    valgrind

# Set the working directory
WORKDIR /workspace

# Default command
CMD ["/bin/sh"]