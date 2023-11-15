#include <iostream>
#include <unistd.h>

int main() {
  const int N = 3;
  int nb_process = 1;
  std::cout << "main pid=" << getpid() << std::endl;

  for (int i = 1, j = N; i <= N && j == N && fork() == 0; i++) {
    std::cout << " i:j " << i << ":" << j << std::endl;
    nb_process = std::min(i + 1, j);
    for (int k = 1; k <= i && j == N; k++) {
      if (fork() == 0) {
        nb_process = 0;
        j = 0;
        std::cout << " k:j " << k << ":" << j << std::endl;
      }
    }
  }

  for (int i = 0; i < nb_process; i++) {
    int res = wait(nullptr);
    if (res == -1)
      std::cout << "trop attendu 😴" << std::endl;
  }

  if (wait(nullptr) != -1)
    std::cout << "pas assez attendu 😤" << std::endl;

  return 0;
}
