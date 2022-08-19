#ifndef _RAYC_STRUTILS_H_
#define _RAYC_STRUTILS_H_ 1

#include <string>
#include <vector>

namespace rayc {

std::vector<std::string> splitstr(const std::string& str, const std::string& delimiter = " ");
std::string strstrip(const std::string& str, char c = ' ');

std::string pathStripEndSeparator(const std::string& str);
std::string pathGetFile(const std::string& str);

namespace {
  template <typename ...Args>
  inline std::string _pathConcat(std::string str) {
    return str;
  }

  template <typename ...Args>
  inline std::string _pathConcat(std::string str, Args... args) {
    if (str.back() != '/') {
      str.push_back('/');
    }
    std::string rest = _pathConcat(args...);
    if (rest.front() == '/') {
      rest.erase(rest.begin());
    }
    return str + rest;
  }
}

template <typename ...Args>
inline std::string pathConcat(Args... args) {
  return _pathConcat(args...);
}

} /* namespace rayc */

#endif /* _RAYC_STRUTILS_H_ */