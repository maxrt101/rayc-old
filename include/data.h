#ifndef _RAYC_DATA_H_
#define _RAYC_DATA_H_ 1

#include <string>

namespace rayc {

enum ResourceType {
  RES_RAYC_CONFIG,
  RES_TEXTURE,
  RES_SPRITE,
  RES_FONT,
  RES_MAP,
};

bool checkDataFolderStructure(std::string folder);

std::string getDataFolder();
void setDataFolder(const std::string& folder);

std::string getResourcePath(ResourceType type, const std::string& name = "");

} /* namespace rayc */

#endif /* _RAYC_DATA_H_ */