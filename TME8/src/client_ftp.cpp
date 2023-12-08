#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

#include "Socket.h"
#include "utils_ftp.h"

void handle_list(int fd, std::array<char, 1024> &buffer) {
  size_t size = read_size(fd);
  int nb = 0;

  while (nb < size) {
    nb += read(fd, &buffer[0], buffer.size());
    std::string entry(buffer.cbegin(), nb);
    std::cout << entry;
  }
}

void handle_upload(int fd, std::array<char, 1024> &buffer, std::string arg) {
  std::ifstream file(arg, std::ifstream::ate | std::ifstream::binary);
  if (!file.is_open()) {
    std::cerr << "Could not open " << arg << std::endl;
    exit(1);
  }

  std::streamsize size = file.tellg();
  if (!write_size(fd, size)) {
    std::cerr << "Could not write file size" << std::endl;
    exit(1);
  }

  file.seekg(0);
  while (!file.eof()) {
    file.read(buffer.data(), buffer.size());
    std::streamsize chunk_size = file.gcount();
    if (chunk_size == 0) {
      std::cerr << "Could not upload file, empty chunk" << std::endl;
      exit(1);
    }

    if (write(fd, buffer.data(), chunk_size) != chunk_size) {
      std::cerr << "Error while sending file" << std::endl;
      exit(1);
    }
  }
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
