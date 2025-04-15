# Proof of Concept: Containerization

After using software like docker a lot I wanted to understand the underlying basics...

> [!WARNING]  
> This project is designed for Linux systems only and requires root privileges to run due to the low-level system operations involved.

## Requirements

- Linux
- cmake
- gcc
- root privileges
- [alpine linux mini root filesystem](https://www.alpinelinux.org/downloads/) located at /container/alpine

## Build Instructions

1. **Clone the repository:**

    ```bash
    git clone https://github.com/xavwe/containerization.git
    cd containerization
    ```

2. **Compile the project:**

    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```

3. **Run the containerization tool:**

    > Ensure you are running as root or with appropriate privileges.

    ```bash
    ./container
    ```

## Learning Resources

- [Liz Rice at GOTO 2018](https://www.youtube.com/watch?v=8fi7uSYlOdc)
