#include "utils_ftp.h"
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <string>
#include <unistd.h>

void parse_command(std::string &input, std::string &action, std::string &arg) {
  if (input.length() >= 4 && input.substr(0, 4) == "EXIT") {
    action = "EXIT";
  } else if (input.length() >= 4 && input.substr(0, 4) == "LIST") {
    action = "LIST";
  } else if (input.length() >= 7 && input.substr(0, 6) == "UPLOAD") {
    action = "UPLOAD";
    arg = input.substr(7, input.length());
  } else if (input.length() >= 9 && input.substr(0, 8) == "DOWNLOAD") {
    action = "DOWNLOAD";
    arg = input.substr(9, input.length());
  }
}

size_t read_size(int fd) {
  size_t size;
  int nb = read(fd, &size, sizeof(size_t));
  if (nb != sizeof(size_t))
    return 0;
  return size;
}

bool write_size(int fd, size_t size) {
  int nb = write(fd, &size, sizeof(size_t));
  if (nb != sizeof(size_t)) {
    return false;
  }
  return true;
}

bool send_file(int fd, std::array<char, 1024> &buffer, const std::string &path,
               std::string &error) {
  std::ifstream file(path, std::ifstream::ate | std::ifstream::binary);
  if (!file.is_open()) {
    error = "Could not open: " + path;
    return false;
  }

  size_t size = file.tellg();
  if (!write_size(fd, size)) {
    error = "Failed to write file size";
    return false;
  }

  file.seekg(0);
  while (!file.eof()) {
    std::fill(std::begin(buffer), std::end(buffer), 0);

    file.read(buffer.data(), buffer.size());
    std::streamsize chunk_size = file.gcount();
    if (chunk_size == 0) {
      error = "Failed to upload file, empty chunk";
      return false;
    }

    if (write(fd, buffer.data(), chunk_size) != chunk_size) {
      error = "Failed to send file chunk";
      return false;
    }
  }

  std::cout << "Uploaded file: " << path << ", size: " << size << std::endl;
  return true;
}

bool receive_file(int fd, std::array<char, 1024> &buffer,
                  const std::string &path, std::string &error) {
  size_t size = read_size(fd);
  if (size == 0) {
    error = "File size of 0 is not supported";
    return false;
  }

  std::ofstream file(path);
  if (!file.is_open()) {
    error = "Failed to create file: " + path;
    return false;
  }

  int total_nb = 0;
  while (total_nb < size) {
    int nb = ::read(fd, buffer.data(), buffer.size());
    if (nb == 0) {
      error = "Failed to read file, empty chunk";
      return false;
    }

    file.write(buffer.data(), nb);
    total_nb += nb;
  }

  std::cout << "Downloaded file: " << path << ", size: " << size << std::endl;
  return true;
}
