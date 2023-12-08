#ifndef SRC_TCPSERVER_H_
#define SRC_TCPSERVER_H_

#include "ConnectionHandler.h"
#include "Pool.h"
#include "ServerSocket.h"
#include <thread>

namespace pr {

// un serveur TCP, la gestion des connections est déléguée
class TCPServer {
  ServerSocket *ss;           // la socket d'attente si elle est instanciee
  ConnectionHandler *handler; // le gestionnaire de session passe a la constru
                              // a completer
  Pool *pool;

public:
  TCPServer(ConnectionHandler *handler) : ss(nullptr), handler(handler) {}
  ~TCPServer();
  // Tente de creer une socket d'attente sur le port donné
  bool startServer(short port);

  // stoppe le serveur
  void stopServer();
};

} // namespace pr

#endif /* SRC_TCPSERVER_H_ */
