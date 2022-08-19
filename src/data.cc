#include <rayc/data.h>
#include <rayc/strutils.h>
#include <rayc/log.h>
#include <filesystem>

static std::string dataFolder;
namespace fs = std::filesystem;

bool rayc::checkDataFolderStructure(std::string folder) {
  folder = pathStripEndSeparator(folder);
  if (!fs::exists(folder + "/rayc.conf")) {
    error("No config file in data folder");
    return false;
  }
  if (!fs::exists(folder + "/textures")) {
    error("No textures folder in data folder");
    return false;
  }
  if (!fs::exists(folder + "/sprites")) {
    error("No sprites folder in data folder");
    return false;
  }
  if (!fs::exists(folder + "/fonts")) {
    error("No fonts folder in data folder");
    return false;
  }
  if (!fs::exists(folder + "/maps")) {
    error("No maps folder in data folder");
    return false;
  }
  return true;
}

std::string rayc::getDataFolder() {
  return dataFolder;
}

void rayc::setDataFolder(const std::string& folder) {
  dataFolder = folder;
}

std::string rayc::getResourcePath(ResourceType type, const std::string& name) {
  switch (type) {
    case RES_RAYC_CONFIG:
      return pathConcat(dataFolder, "rayc.conf");
    case RES_TEXTURE:
      return pathConcat(dataFolder, "textures", name);
    case RES_SPRITE:
      return pathConcat(dataFolder, "sprites", name);
    case RES_FONT:
      return pathConcat(dataFolder, "fonts", name);
    case RES_MAP:
      return pathConcat(dataFolder, "maps", name);
    default:
      return "";
  }
}
