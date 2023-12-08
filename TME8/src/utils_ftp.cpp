#include "utils_ftp.h"

void parse_command(const std::string &input, std::string &action,
                   std::string &arg) {
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
