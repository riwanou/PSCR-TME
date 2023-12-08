#include "src/TCPServer.h"
#include "src/Job.h"
#include "src/Pool.h"
#include "src/ServerSocket.h"
#include <signal.h>
#include <sys/errno.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <unistd.h>

pr::TCPServer::~TCPServer() {
  delete pool;
  delete handler;
  delete ss;
}

bool pr::TCPServer::startServer(short port) {
  ss = new ServerSocket(port);
  if (!ss->isOpen()) {
    perror("tcpServer::startServer isOpen");
    exit(1);
  }

  pool = new Pool(100);
  pool->start(20);
  std::cout << "TCP server started on port " << port << std::endl;

  // abort signal handling with a pipe to notify select
  static int abort = 0;
  static int pipe_fd[2];
  pipe(pipe_fd);

  // send some bytes into the pipe, trigger the select on pipe fd read
  signal(SIGINT, [](int sig) {
    // send signal to select
    if (write(pipe_fd[0], &abort, sizeof(int)) == 0) {
      perror("tcpServer::startServer signal pipe write");
      exit(1);
    }
    close(pipe_fd[0]);
  });

  // select file descriptor set
  fd_set set;
  int max_fd = ss->getFD();
  max_fd = std::max(max_fd, pipe_fd[1]);

  while (true) {
    FD_ZERO(&set);
    FD_SET(ss->getFD(), &set);
    FD_SET(pipe_fd[1], &set);

    int nready = select(max_fd + 1, &set, NULL, NULL, NULL);
    if (nready < 0 && errno != EINTR) {
      perror("tcpServer::startServer select");
      exit(1);
    }
    if (nready == 0)
      continue;

    // interupt signal received
    if (FD_ISSET(pipe_fd[1], &set)) {
      std::cout << "tcp server gracefully interrupted" << std::endl;
      close(pipe_fd[1]);
      return false;
    }

    // server tcp socket updated
    if (FD_ISSET(ss->getFD(), &set)) {
      Socket client = ss->accept();
      pool->addJob(new HandleJob(handler, client));
    }
  }
}

void pr::TCPServer::stopServer() {
  std::cout << "TCP server stopped" << std::endl;
  pool->stop();
  ss->close();
}
