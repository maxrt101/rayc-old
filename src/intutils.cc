#include <rayc/intutils.h>
#include <rayc/log.h>
#include <exception>

bool rayc::stoi(const std::string& str, int& result) {
  try {
    result = std::stoi(str);
    return true;
  } catch (std::invalid_argument& e) {
    error("Cannot convert '%s' to int: %s", str.c_str(), e.what());
    return false;
  } catch (std::out_of_range& e) {
    error("Cannot convert '%s' to int: %s", str.c_str(), e.what());
    return false;
  }
}

bool rayc::stof(const std::string& str, float& result) {
  try {
    result = std::stod(str);
    return true;
  } catch (std::invalid_argument& e) {
    error("Cannot convert '%s' to int: %s", str.c_str(), e.what());
    return false;
  } catch (std::out_of_range& e) {
    error("Cannot convert '%s' to int: %s", str.c_str(), e.what());
    return false;
  }
}

