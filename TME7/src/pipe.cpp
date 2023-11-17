#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

const std::string ERR_ARG = "err: need at least 2 arguments separated with `|`";

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cerr << ERR_ARG << std::endl;
    exit(1);
  }

  int pipe_index = 0;
  for (int i = 0; i < argc; i++) {
    if (argv[i][0] == '|') {
      pipe_index = i;
      break;
    }
  }

  if (pipe_index == argc - 1) {
    std::cerr << ERR_ARG << std::endl;
    exit(1);
  }

  // pipe creation
  int fd[2];
  pipe(fd);

  // child creation
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(1);
  }

  if (pid == 0) {
    // child
    close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);
    argv[pipe_index] = nullptr;
    execv(argv[1], argv + 1);
  }

  // parent
  close(fd[1]);
  dup2(fd[0], STDIN_FILENO);
  pipe_index++;
  execv(argv[pipe_index], argv + pipe_index);

  return 0;
}
