#include "TCPServer.h"
#include <iostream>
#include <unistd.h>

#include "src/ConnectionHandler.h"
#include "src/Socket.h"
#include "src/TCPServer.h"
#include <array>
#include <dirent.h>
#include <sys/types.h>

void write_end(int fd) {
  char end = '0';
  write(fd, &end, sizeof(char));
}

class ServerFTP : public pr::ConnectionHandler {
private:
  std::string directory;
  std::array<char, 1024> buffer;

public:
  ServerFTP(const std::string &directory) : directory(directory) {}

  void handleConnection(pr::Socket socket) {
    int fd = socket.getFD();
    std::string ack = "ACK";

    while (true) {
      std::cout << "Listening for client request" << std::endl;
      // client request
      int nb = ::read(fd, &buffer[0], buffer.size());

      // client not connected anymore
      if (nb == 0) {
        std::cout << "Client connexion end" << std::endl;
        break;
      }

      // ACK
      if (write(fd, ack.c_str(), ack.size()) != ack.size()) {
        perror("Could not write ACK");
        exit(1);
      }

      // handle client request
      std::string action(buffer.cbegin(), nb);
      if (action == "LIST") {
        handleList(fd);
      } else if (action == "UPLOAD") {
        handleUpload(fd);
      } else if (action == "DOWNLOAD") {
        handleDownload(fd);
      } else {
        std::cout << "Failed to recognize client request" << std::endl;
        break;
      }
    }
  }

  void write_size(int fd, size_t size) {
    int nb = write(fd, &size, sizeof(size_t));
    if (nb != sizeof(size_t)) {
      perror("Could not write size");
      exit(1);
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

    write_size(fd, dir_size);

    for (const auto &entry : entries) {
      std::string val = entry + "\n";
      if (entry.size() > 0 &&
          write(fd, val.c_str(), val.size()) != val.size()) {
        perror("handleList: could not write directory entry");
        exit(1);
      }
    }
  }

  void handleUpload(int fd) {}
  void handleDownload(int fd) {}

  pr::ConnectionHandler *clone() const { return new ServerFTP(directory); }
};

int main() {
  pr::TCPServer server(new ServerFTP("/tmp"));
  server.startServer(1668);
  server.stopServer();
  return 0;
}
