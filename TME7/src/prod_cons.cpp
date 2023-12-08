#include "Stack.h"
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

// Warning: does not work on macos :D

using namespace std;
using namespace pr;

void producteur(Stack<char> *stack) {
  char c;
  while (cin.get(c)) {
    stack->push(c);
  }
}

void consomateur(Stack<char> *stack) {
  while (true) {
    char c = stack->pop();
    cout << c << flush;
  }
}

int main() {
  pid_t pp, pc;
  void *ptr;
  Stack<char> *s;
  int shm_fd;

  size_t stack_size = sizeof(Stack<char>);

  // init shader memory segment
  shm_fd = shm_open("/shared_memory", O_RDWR | O_CREAT | O_EXCL, 0666);
  if (ftruncate(shm_fd, stack_size) < 0) {
    perror("truncate");
    goto shm_prob;
  }
  if (shm_fd < 0) {
    perror("shm_open");
    goto shm_prob;
  }

  // load memory segment into memory
  ptr = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    goto mmap_prob;
  }

  s = new (ptr) Stack<char>();

  pp = fork();
  if (pp == 0) {
    producteur(s);
    return 0;
  }

  pc = fork();
  if (pc == 0) {
    consomateur(s);
    return 0;
  }

  // wait for childs
  wait(0);
  wait(0);

  // destroy resources
  s->~Stack();

mmap_prob:
  if (munmap(ptr, stack_size) < 0) {
    perror("unmap");
  }
shm_prob:
  if (shm_unlink("/shared_memory") < 0) {
    perror("shm_unlink");
  }

  return 0;
}
