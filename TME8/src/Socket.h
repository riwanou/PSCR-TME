#ifndef SRC_SOCKET_H_
#define SRC_SOCKET_H_

#include <iosfwd>
#include <iostream>
#include <netdb.h>
#include <netinet/ip.h>
#include <string>

namespace pr {

class Socket {
  int fd;

public:
  Socket() : fd(-1) {}
  Socket(int fd) : fd(fd) {}

  // tente de se connecter à l'hôte fourni
  void connect(const std::string &host, short port);
  void connect(sockaddr *ip, size_t addrlen);

  bool isOpen() const { return fd != -1; }
  int getFD() { return fd; }

  void close();
};

inline std::ostream &operator<<(std::ostream &os, struct sockaddr_in *addr) {
  char hname[1024];
  if (!getnameinfo((struct sockaddr *)addr, sizeof *addr, hname, 1024, nullptr,
                   0, 0)) {
    os << hname;
  }
  return os;
}

} // namespace pr

#endif /* SRC_SOCKET_H_ */
