#include <fstream>

inline std::string readFromFile(std::string path) {
  std::string content;
  std::getline(std::ifstream(path), content, '\0');

  return content;
}

