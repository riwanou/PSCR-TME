#include <cstdio>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "Socket.h"

std::ostream &pr::operator<<(std::ostream &os, struct sockaddr_in *addr) {
  char hname[1024];
  if (!getnameinfo((struct sockaddr *)addr, sizeof *addr, hname, 1024, nullptr,
                   0, 0)) {
    os << hname;
  }
  return os;
}

void pr::Socket::connect(const std::string &host, short port) {
  struct addrinfo hints, *result;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  auto port_str = std::to_string(port);

  int err = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &result);
  if (err) {
    perror("socket::connect getaddrinfo");
    exit(1);
  }

  connect(result->ai_addr, sizeof(*result->ai_addr));
  freeaddrinfo(result);
}

void pr::Socket::connect(sockaddr *addr, size_t addrlen) {
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    perror("socket::connect socket");
    exit(1);
  }

  if (::connect(fd, addr, addrlen) < 0) {
    ::close(fd);
    perror("socket::connect connect");
    exit(1);
  }
}

void pr::Socket::close() {
  if (shutdown(fd, 0) < 0) {
    perror("socket::close shutdown");
  }
  if (::close(fd) < 0) {
    perror("socket::close close");
  }
}
