#ifndef _RAYC_CONFIG_H_
#define _RAYC_CONFIG_H_ 1

#include <map>
#include <string>
#include <functional>

namespace rayc {

class Config {
 public:
  using Section = std::map<std::string, std::string>;

 private:
  mutable std::map<std::string, Section> m_sections;

 public:
  Config() = default;
  ~Config() = default;

  bool exists(const std::string& section, const std::string& key) const;
  void ifExists(const std::string& section, const std::string& key, std::function<void(const std::string&)> cb);

  Section& getSection(const std::string& section) const;
  std::string getValue(const std::string& section, const std::string& key) const;
  std::string getValueOr(const std::string& section, const std::string& key, const std::string& defaultValue) const;
  std::string getValueOrDie(const std::string& section, const std::string& key, const std::string& errorMsg) const;
  void setValue(const std::string& section, const std::string& key, const std::string& value);

  Section& operator[](const std::string& section);

  std::string toString() const;
  void toFile(const std::string& fileName) const;

  static Config fromFile(const std::string& fileName);
  static Config fromString(const std::string& src);
};

} /* namespace rayc */

#endif /* _RAYC_CONFIG_H_ */