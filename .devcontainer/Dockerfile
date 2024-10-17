# Use a lightweight Linux image
FROM alpine:latest

# Install necessary tools and libraries
RUN apk update && apk add --no-cache \
    build-base \
    boost-dev \
    git \
    openssh

# Set the working directory
WORKDIR /workspace

# Default command
CMD ["/bin/sh"]