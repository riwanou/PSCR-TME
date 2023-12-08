#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

#include "Socket.h"
#include "utils_ftp.h"

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

void handle_upload(int fd, std::array<char, 1024> &buffer, std::string arg) {
  std::ifstream file(arg);
  std::streamsize size = file.tellg();
  std::cout << "file: " << arg << ", size: " << size << std::endl;
}

void handle_download(int fd, std::array<char, 1024> &buffer, std::string arg) {
  std::cout << "ARG: " << arg << std::endl;
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
    std::string input, action, arg;
    std::getline(std::cin, input);
    parse_command(input, action, arg);

    if (action == "EXIT") {
      return 0;
    }

    // server request
    write(fd, input.c_str(), input.size());

    // server response
    int nb = read(fd, &buffer[0], 3);
    std::string receiv(buffer.cbegin(), nb);

    // end of connection
    if (nb == 0 || receiv != "ACK") {
      std::cout << "server connexion end" << std::endl;
      return 0;
    }

    // handle server response
    if (action == "LIST") {
      handle_list(fd, buffer);
    } else if (action == "UPLOAD" && arg.length() > 0) {
      handle_upload(fd, buffer, arg);
    } else if (action == "DOWNLOAD" && arg.length() > 0) {
      handle_download(fd, buffer, arg);
    } else {
      std::cout << "Request not supported, please use one of: EXIT, LIST, "
                   "UPLOAD <file>, DOWNLOAD <file>"
                << std::endl;
    }
  }
}
