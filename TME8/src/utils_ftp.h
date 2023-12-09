#pragma once

#include <iostream>
#include <string>

void parse_command(std::string &input, std::string &action, std::string &arg);

size_t read_size(int fd);
bool write_size(int fd, size_t size);

bool send_file(int fd, std::array<char, 1024> &buffer, const std::string &path,
               std::string &error);
bool receive_file(int fd, std::array<char, 1024> &buffer,
                  const std::string &path, std::string &error);
