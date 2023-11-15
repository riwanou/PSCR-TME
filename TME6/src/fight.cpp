#include <csignal>
#include <iostream>
#include <sys/signal.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void randsleep() {
  int r = rand();
  double ratio = (double)r / (double)RAND_MAX;
  struct timespec tosleep;
  tosleep.tv_sec = 0;
  // 300 millions de ns = 0.3 secondes
  tosleep.tv_nsec = 300000000 + ratio * 700000000;
  struct timespec remain;
  while (nanosleep(&tosleep, &remain) != 0) {
    tosleep = remain;
  }
}

// static variables
bool is_child = false;
std::string name = "Vador";
int life = 3;

void attack(pid_t target) {
  // handle signal
  // we lose life when we receive a signal from the opponent
  signal(SIGINT, [](int sig) {
    life--;
    std::cout << name << " -> received sig " << sig << " - remaining life "
              << life << std::endl;
    // dead boo
    if (life == 0) {
      std::cout << name << " is dead!" << std::endl;
      exit(1);
    }
  });
  // if we do not wait, then the child will stay a zombie. In this case,
  // kill will never give -1 and the program never stop.
  // we use WNOHANG to not block here
  waitpid(target, nullptr, WNOHANG);
  // if kill give -1, then the other process is dead
  int res = kill(target, SIGINT);
  if (res < 0) {
    std::cout << name << " killed his target!" << std::endl;
    exit(0);
  }
  randsleep();
}

void defend() {
  // modified defense for child
  if (is_child) {
    struct sigaction action;
    action.sa_handler = [](int) {
      std::cout << "parry from child, life(" << life << ")" << std::endl;
    };
    sigaction(SIGINT, &action, NULL);

    // mask signals
    sigset_t mask; // old_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL); //&old_mask);

    // sleep
    randsleep();

    // check if attack occur, remove mask
    std::cout << "wait for signal (child)" << std::endl;
    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGINT);
    sigsuspend(&set);
    // sigprocmask(SIG_UNBLOCK, &mask, NULL);

  } else {
    // we ignore the signal, so no life is lost when received
    signal(SIGINT, SIG_IGN);
    randsleep();
  }
}

void fight(pid_t target) {
  while (true) {
    defend();
    attack(target);
  }
}

int main() {
  pid_t pid, target, parent;
  parent = getpid();

  // negation, everything except what's unset
  sigset_t set;
  sigfillset(&set);
  sigdelset(&set, SIGINT);

  pid = fork();
  if (pid < 0) {
    perror("child creation");
    return -1;
  }

  if (pid == 0) {
    // child
    name = "Luke";
    target = parent;
    is_child = true;
  } else {
    // parent
    target = pid;
  }

  srand(time(NULL) + pid);
  fight(target);

  wait(nullptr);
}
