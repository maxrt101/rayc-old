#ifndef _RAYC_INTUTILS_H_
#define _RAYC_INTUTILS_H_ 1

#include <string>

namespace rayc {

bool stoi(const std::string& str, int& result);
bool stof(const std::string& str, float& result);

} /* namespace rayc */

#endif /* _RAYC_INTUTILS_H_ */