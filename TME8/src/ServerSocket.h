#ifndef SRC_SERVERSOCKET_H_
#define SRC_SERVERSOCKET_H_

#include "Socket.h"

namespace pr {

class ServerSocket {
  int socketfd;

public:
  // Demarre l'ecoute sur le port donne
  ServerSocket(short port);

  int getFD() { return socketfd; }
  bool isOpen() const { return socketfd != -1; }

  Socket accept();

  void close();
};

} // namespace pr

#endif /* SRC_SERVERSOCKET_H_ */
