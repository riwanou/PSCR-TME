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
        handleDownload(fd);
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
    size_t size = read_size(fd);
    if (size == 0) {
      std::cout << "File size of 0 is not supported" << std::endl;
      return;
    }

    std::filesystem::path path(arg);
    std::ofstream file(directory + "/" + path.filename().string());
    if (!file.is_open()) {
      std::cout << "Could not create file: " << arg << std::endl;
      return;
    }

    int nb = 0;
    while (nb < size) {
      nb += ::read(fd, &buffer[0], buffer.size());
      if (nb == 0) {
        perror("handleUpload: read client upload");
        exit(1);
      }
      file.write(buffer.cbegin(), nb);
    }

    std::cout << "Uploaded file: " << arg << ", size: " << size << std::endl;
  }

  void handleDownload(int fd) {}

  pr::ConnectionHandler *clone() const { return new ServerFTP(directory); }
};

int main() {
  pr::TCPServer server(new ServerFTP("/tmp"));
  server.startServer(1668);
  server.stopServer();
  return 0;
}
