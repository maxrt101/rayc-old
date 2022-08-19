#include <rayc/config.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <rayc/app.h>
#include <rayc/log.h>
#include <rayc/strutils.h>

static std::vector<std::string> splitLines(const std::string& str) {
  std::vector<std::string> lines;
  std::string line;
  for (int i = 0; i < str.size(); i++) {
    if (str[i] == '\n') {
      lines.push_back(line);
      line.clear();
    } else {
      line.push_back(str[i]);
    }
  }
  return lines;
}

bool rayc::Config::exists(const std::string& section, const std::string& key) const {
  if (m_sections.find(section) != m_sections.end()) {
    if (m_sections[section].find(key) != m_sections[section].end()) {
      return true;
    }
  }
  return false;
}

void rayc::Config::ifExists(const std::string& section, const std::string& key, std::function<void(const std::string&)> cb) {
  if (m_sections.find(section) != m_sections.end()) {
    auto itr = m_sections[section].find(key);
    if (itr != m_sections[section].end()) {
      cb(itr->second);
    }
  }
}

rayc::Config::Section& rayc::Config::getSection(const std::string& section) const {
  return m_sections[section];
}

std::string rayc::Config::getValue(const std::string& section, const std::string& key) const {
  return m_sections[section][key];
}

std::string rayc::Config::getValueOr(const std::string& section, const std::string& key, const std::string& defaultValue) const {
  if (m_sections.find(section) != m_sections.end()) {
    if (m_sections[section].find(key) != m_sections[section].end()) {
      return m_sections[section][key];
    }
  }
  return defaultValue;
}

std::string rayc::Config::getValueOrDie(const std::string& section, const std::string& key, const std::string& errorMsg) const {
  if (m_sections.find(section) != m_sections.end()) {
    if (m_sections[section].find(key) != m_sections[section].end()) {
      return m_sections[section][key];
    }
  }
  rayc::error("Failed to get %s.%s config key: %s", section.c_str(), key.c_str(), errorMsg.c_str());
  die();
}

void rayc::Config::setValue(const std::string& section, const std::string& key, const std::string& value) {
  m_sections[section][key] = value;
}

rayc::Config::Section& rayc::Config::operator[](const std::string& section) {
  return m_sections[section];
}

std::string rayc::Config::toString() const {
  std::stringstream ss;
  for (auto& [sectionName, section] : m_sections) {
    ss << "[" << sectionName << "]\n";
    for (auto& [key, value] : section) {
      ss << key << " = " << value << "\n";
    }
    ss << "\n";
  }
  return ss.str();
}

void rayc::Config::toFile(const std::string& fileName) const {
  std::ofstream file(fileName);
  if (!file) {
    fatal("Can't open file '%s'", fileName.c_str());
    die();
  }
  file << toString();
}

rayc::Config rayc::Config::fromFile(const std::string& fileName) {
  std::ifstream file(fileName);
  if (!file) {
    fatal("Can't open file '%s'", fileName.c_str());
    die();
  }
  std::stringstream ss;
  ss << file.rdbuf();
  return Config::fromString(ss.str());
}

rayc::Config rayc::Config::fromString(const std::string& src) {
  std::vector<std::string> lines = splitLines(src);
  std::string currentSection;
  Config config;

  int lineNumber = 0;
  for (auto& line : lines) {
    lineNumber++;
    if (!line.size() || line.front() == '#') continue;

    if (line.front() == '[') {
      if (line.back() == ']') {
        currentSection = line.substr(1, line.size()-2);
      } else {
        fatal("Config Parsing Error:\n    Near '%s' (line %d)\n    Expected ']' at the end of section name", line.c_str(), lineNumber);
        die();
      }
    } else {
      int i = line.find("=");
      if (i != std::string::npos) {
        std::string key = strstrip(line.substr(0, i)), value = strstrip(line.substr(i+1));
        config.setValue(currentSection, key, value);
      } else {
        fatal("Config Parsing Error:\n    Near '%s' (line %d)\n    Expected key-value pair", line.c_str(), lineNumber);
        die();
      }
    }
  }
  return config;
}
