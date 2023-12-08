#include "Socket.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <unistd.h>

size_t read_size(int fd) {
  size_t size;
  int nb = read(fd, &size, sizeof(size_t));
  if (nb != sizeof(size_t))
    return 0;
  return size;
}

void handle_list(int fd, std::array<char, 1024> &buffer) {
  size_t size = read_size(fd);
  int total_nb = 0;

  while (total_nb < size) {
    int nb = read(fd, &buffer[0], buffer.size());
    total_nb += nb;

    std::string entry(buffer.cbegin(), nb);
    std::cout << entry;
  }
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
    // parse request
    std::string action;
    std::getline(std::cin, action);

    // server request
    write(fd, action.c_str(), action.size());

    // server response
    int nb = read(fd, &buffer[0], 3);
    std::string receiv(buffer.cbegin(), nb);

    // end of connection
    if (nb == 0 || receiv != "ACK") {
      std::cout << "server connexion end" << std::endl;
      return 0;
    }

    // handle server response
    if (action == "EXIT") {
      return 0;
    } else if (action == "LIST") {
      handle_list(fd, buffer);
    } else {
      std::cout << "Request not supported, please use one of: EXIT, LIST"
                << std::endl;
    }
  }
}
