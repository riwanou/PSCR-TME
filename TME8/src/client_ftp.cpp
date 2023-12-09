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

  std::cout << std::endl;
}

void handle_upload(int fd, std::array<char, 1024> &buffer,
                   const std::string &arg) {
  std::string error;
  if (!send_file(fd, buffer, arg, error)) {
    std::cerr << "Upload error: " << error << std::endl;
    exit(1);
  }
}

void handle_download(int fd, std::array<char, 1024> &buffer,
                     const std::string &arg) {
  std::string path = "./" + arg;

  std::string error;
  if (!receive_file(fd, buffer, path, error)) {
    std::cerr << "Handle download error: " << error;
    exit(1);
  }
}

int main(int argc, char **argv) {
  // parse arguments
  if (argc < 2) {
    std::cerr << "Failed to parse arguments, need <port>" << std::endl;
    exit(1);
  }
  short port = std::stoi(argv[1]);

  // connect
  pr::Socket sock;
  sock.connect("localhost", port);
  if (!sock.isOpen()) {
    perror("Failed to connect to server");
    exit(1);
  }

  int fd = sock.getFD();
  std::array<char, 1024> buffer;

  while (true) {
    std::fill(std::begin(buffer), std::end(buffer), 0);

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
