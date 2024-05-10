#include <sstream>
#include <vector>
#include <string>

#include "Util.h"

std::vector<std::string> split_message(const std::string &message, const std::string& delim) {
  std::vector<std::string> toks;
  std::stringstream ss = std::stringstream{message};
  std::string line;
  while (getline(ss, line, *delim.begin())) {
    toks.push_back(line);
    ss.ignore(delim.length() - 1);
  }
  return toks;
}