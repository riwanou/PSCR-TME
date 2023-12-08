#include "Socket.h"
#include <iostream>
#include <string>
#include <unistd.h>

void handleList(int fd, std::array<char, 1024> &buffer, int nb) {
  do {
    std::string entry(buffer.cbegin(), nb);
    std::cout << entry;
    nb = read(fd, &buffer[0], buffer.size());
  } while (nb > 0);
}

int main() {
  pr::Socket sock;
  sock.connect("localhost", 1668);
  if (!sock.isOpen()) {
    perror("Failed to connect to server");
    exit(1);
  }

  int fd = sock.getFD();
  std::array<char, 1024> buffer;

  while (true) {
    // server request
    std::string action = "LIST";
    write(fd, action.c_str(), action.size());

    // server response
    std::string receiv;
    int nb = read(fd, &buffer[0], buffer.size());

    // end of connection
    if (nb == 0) {
      std::cout << "server connexion end" << std::endl;
      return 0;
    }

    // handle server response
    if (action == "LIST") {
      handleList(fd, buffer, nb);
    }
  }
}
