#include "src/TCPServer.h"
#include "src/Job.h"
#include "src/Pool.h"
#include "src/ServerSocket.h"

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

  while (true) {
    Socket client = ss->accept();
    pool->addJob(new HandleJob(handler, client));
  }

  pool->stop();
  ss->close();
}

void pr::TCPServer::stopServer() {}
