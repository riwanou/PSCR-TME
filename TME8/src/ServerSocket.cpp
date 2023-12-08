#include <netinet/in.h>
// #include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <unistd.h>

#include "src/ServerSocket.h"
#include "src/Socket.h"

pr::ServerSocket::ServerSocket(short port) {
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0) {
    perror("serverSocket::serverSocket socket");
    exit(1);
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_UNSPEC;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0

  if (bind(socketfd, (struct sockaddr *)&addr, sizeof(addr)) > 0) {
    ::close(socketfd);
    perror("serverSocket::serverSocket bind");
    exit(1);
  }

  listen(socketfd, 50); // wait queue
}

pr::Socket pr::ServerSocket::accept() {
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(sockaddr_in);
  int clientfd = ::accept(socketfd, (struct sockaddr *)&addr, &addrlen);
  std::cout << "accept connexion: " << &addr << std::endl;
  return pr::Socket(clientfd);
}

void pr::ServerSocket::close() {
  if (shutdown(socketfd, 0) < 0) {
    perror("serverSocket::close shutdown");
  }
  if (::close(socketfd) < 0) {
    perror("serverSocket::close close");
  }
}
