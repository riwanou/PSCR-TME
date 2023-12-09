#include <array>
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

#include "TCPServer.h"
#include "src/ConnectionHandler.h"
#include "src/Socket.h"
#include "src/TCPServer.h"
#include "src/utils_ftp.h"

void write_end(int fd) {
  char end = '0';
  write(fd, &end, sizeof(char));
}

std::string ack = "ACK";
void write_ack(int fd) {
  if (write(fd, ack.c_str(), ack.size()) != ack.size()) {
    perror("Could not write ACK");
    exit(1);
  }
}

class ServerFTP : public pr::ConnectionHandler {
private:
  std::string directory;
  std::array<char, 1024> buffer;

public:
  ServerFTP(const std::string &directory) : directory(directory) {}

  void handleConnection(pr::Socket socket) {
    int fd = socket.getFD();

    while (true) {
      std::fill(std::begin(buffer), std::end(buffer), 0);
      std::cout << "Listening for client request" << std::endl;

      // client request
      int nb = ::read(fd, &buffer[0], buffer.size());

      // client not connected anymore
      if (nb == 0) {
        std::cout << "Client connexion end" << std::endl;
        break;
      }

      std::string input(buffer.cbegin(), nb);
      std::string action, arg;
      parse_command(input, action, arg);

      // handle client request
      if (action == "LIST") {
        write_ack(fd);
        handleList(fd);
      } else if (action == "UPLOAD") {
        write_ack(fd);
        handleUpload(fd, arg);
      } else if (action == "DOWNLOAD") {
        write_ack(fd);
        handleDownload(fd, arg);
      } else {
        std::cout << "Failed to recognize client request" << std::endl;
        int bloup = 0;
        write(fd, &bloup, sizeof(int));
        break;
      }
    }
  }

  // List files in directory
  void handleList(int fd) {
    std::cout << "RECEIV: action LIST" << std::endl;

    DIR *dirp = opendir(directory.c_str());
    if (dirp == NULL) {
      perror("handleList: opendir");
      exit(1);
    }

    size_t dir_size = 0;
    struct dirent *dp;
    std::vector<std::string> entries;
    while ((dp = readdir(dirp)) != NULL) {
      entries.emplace_back(dp->d_name);
      dir_size += dp->d_namlen;
    }
    closedir(dirp);

    if (!write_size(fd, dir_size)) {
      perror("handleList: could not write size");
      exit(1);
    }

    for (const auto &entry : entries) {
      std::string val = entry + "\n";
      if (entry.size() > 0 &&
          write(fd, val.c_str(), val.size()) != val.size()) {
        perror("handleList: could not write directory entry");
        exit(1);
      }
    }
  }

  void handleUpload(int fd, const std::string &arg) {
    std::filesystem::path p(arg);
    std::string path = directory + "/" + p.filename().string();

    std::string error;
    if (!receive_file(fd, buffer, path, error)) {
      std::cerr << "Handle upload error: " << error;
      return;
    }
  }

  void handleDownload(int fd, const std::string &arg) {
    std::string path = directory + "/" + arg;

    std::string error;
    if (!send_file(fd, buffer, path, error)) {
      std::cerr << "Upload error: " << error << std::endl;
      exit(1);
    }
  }

  pr::ConnectionHandler *clone() const { return new ServerFTP(directory); }
};

int main(int argc, char **argv) {
  // parse arguments
  if (argc < 3) {
    std::cerr << "Failed to parse arguments, need <port> <directory>"
              << std::endl;
    exit(1);
  }

  short port = std::stoi(argv[1]);

  std::string directory = argv[2];
  std::ifstream dir(directory);
  if (!dir.is_open()) {
    std::cerr << "Directory does not exist" << std::endl;
    exit(1);
  }

  // start server
  pr::TCPServer server(new ServerFTP(directory));
  server.startServer(port);
  server.stopServer();
  return 0;
}
