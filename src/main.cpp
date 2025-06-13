#include <iostream>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <sys/mount.h>

constexpr int STACK_SIZE = 1024 * 1024;

int initialize(void *arg);

const char * newRoot = "/container/alpine";

int main() {
    // Check for root permission
    if (getuid())
    {
        std::cerr << "You need root permissions to create a container!\n";
        return 1;
    }

    // Check for new root filesystem
    if (access(newRoot, F_OK) != 0)
    {
        std::cerr << "container root path not found\n";
        return 1;
    }

    std::cout << "Creating container..." << std::endl;

    // Dynamically allocate memory for the child stack
    void *stack = malloc(STACK_SIZE);
    if (!stack) {
        std::cerr << "malloc failed\n";
        return 1;
    }

    // Create a new UTS, PID, and Mount namespace
    pid_t pid = clone(initialize,
                      static_cast<char*>(stack) + STACK_SIZE,
                      CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD,
                      nullptr);
    if (pid == -1) {
        std::cerr << "clone failed\n";
        free(stack);
        return 1;
    }

    std::cout << "Entering container..." << "\n";

    // Wait for the child process to terminate
    if (waitpid(pid, nullptr, 0) == -1) {
        std::cerr << "waitpid failed\n";
    }
    std::cout << "Exited container...\n";

    free(stack);
    return 0;
}

int initialize(void *arg) {
    // Set hostname inside the UTS namespace
    if (sethostname("isolated-container", strlen("isolated-container")) == -1) {
        std::cerr << "sethostname failed\n";
        return 1;
    }

    // Change root to the new container filesystem
    if (chroot(newRoot) != 0) {
        std::cerr << "chroot failed\n";
        return 1;
    }

    // Change working directory to the new root
    if (chdir("/") != 0) {
        std::cerr << "chdir failed\n";
        return 1;
    }

    // Mount a new proc filesystem inside the container's /proc directory
    if (mount("proc", "/proc", "proc", 0, nullptr) != 0) {
        std::cerr << "mount /proc failed\n";
        return 1;
    }

    // Fork a new process to run the shell so that we can unmount /proc afterwards.
    pid_t shell_pid = fork();
    if (shell_pid < 0) {
        std::cerr << "fork failed\n";
        // Attempt to clean up the mount if fork fails.
        if (umount("/proc") != 0) {
            std::cerr << "umount /proc failed\n";
        }
        return 1;
    }

    if (shell_pid == 0) {
        // Child process: execute the shell
        execlp("/bin/sh", "sh", nullptr);
        std::cerr << "execlp failed\n";
        exit(1);
    }

    // Parent process: wait for the shell to exit
    int status;
    if (waitpid(shell_pid, &status, 0) == -1) {
        std::cerr << "waitpid failed\n";
    }

    // Unmount /proc after the shell has exited
    if (umount("/proc") != 0) {
        std::cerr << "umount /proc failed\n";
        return 1;
    }

    return 0;
}
