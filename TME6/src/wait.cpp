#include <csignal>
#include <iostream>
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>

// suspend current processus execution until child with given pid is exited
int wait_till_pid(pid_t pid) {
  if (kill(pid, 0))
    return -1;
  while (true) {
    if (kill(pid, 0))
      return 0;
    wait(nullptr);
  }
}

// version with signals
int wait_till_pid(pid_t pid, int sec) {

  static int wait_pid = pid;
  static bool stop_waiting = false;

  // wait till child with pid stop
  struct sigaction action;
  action.sa_flags = 0;
  sigemptyset(&action.sa_mask);
  action.sa_handler = [](int sig) {
    int pid = wait(nullptr);
    if (wait_pid == pid) {
      stop_waiting = true;
    }
  };
  sigaction(SIGCHLD, &action, NULL);

  // timeout with alarm 
  struct sigaction alarm_action;
  alarm_action.sa_flags = 0;
  sigemptyset(&alarm_action.sa_mask);
  alarm_action.sa_handler = [](int sig) { stop_waiting = true; };
  alarm(sec);

  sigset_t set;
  sigemptyset(&set);

  while (!stop_waiting)
    sigsuspend(&set);

  return 0;
}

int main() {
  pid_t pid, pid2;

  pid = fork();

  if (pid == 0) {
    // child 1
    sleep(4);
    std::cout << "end of child 1" << std::endl;
  } else {
    // parent
    pid2 = fork();
    if (pid2 > 0) {
      int res = wait_till_pid(pid, 5);
      std::cout << "wait: " << res << std::endl;
    } else {
      sleep(2);
      std::cout << "end of child 2" << std::endl;
    }
  }

  return 0;
}
