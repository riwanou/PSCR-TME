#pragma once

#include <iostream>
#include <string>

void parse_command(const std::string &input, std::string &action,
                   std::string &arg);

size_t read_size(int fd);
bool write_size(int fd, size_t size);
