#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

static int error(char *message)
{
    int length = 0;
    while (message[length] != '\0') {
        ++length;
    }
    write(STDERR_FILENO, message, length);
    return 1;
}

int main(void)
{
    char filename[1024], c;
    size_t length = 0;
    ssize_t count;

    write(STDERR_FILENO, "File: ", 6);
    while ((count = read(STDIN_FILENO, &c, 1)) > 0 && c != '\n') {
        if (length == sizeof(filename) - 1 || c == '\0') {
            return error("Invalid filename\n");
        }
        filename[length++] = c;
    }
    if (count < 0) {
        return error("Cannot read filename\n");
    }
    if (length > 0 && filename[length - 1] == '\r') {
        --length;
    }
    filename[length] = '\0';

    int file = open(filename, O_RDONLY);
    if (file < 0) {
        return error("Cannot open file\n");
    }
    int pipe1[2];
    if (pipe(pipe1) < 0) {
        return error("Cannot create pipe\n");
    }
    pid_t pid = fork();
    if (pid < 0) {
        return error("Cannot create process\n");
    }
    if (pid == 0) {
        close(pipe1[0]);
        if (dup2(file, STDIN_FILENO) < 0 || dup2(pipe1[1], STDOUT_FILENO) < 0) {
            _exit(error("Cannot redirect input/output\n"));
        }
        close(file);
        close(pipe1[1]);
        char *args[] = {"./child", NULL};
        execve("./child", args, environ);
        _exit(error("Cannot start child\n"));
    }

    close(file);
    close(pipe1[1]);
    int result = 0;
    while ((count = read(pipe1[0], &c, 1)) > 0) {
        if (write(STDOUT_FILENO, &c, 1) != 1) {
            result = error("Cannot write result\n");
            break;
        }
    }
    if (count < 0) {
        result = error("Cannot read pipe\n");
    }
    close(pipe1[0]);

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        return error("Cannot wait for child\n");
    }
    return result || !WIFEXITED(status) || WEXITSTATUS(status) != 0;
}
