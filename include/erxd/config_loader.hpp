#pragma once

#include <string>

#include <erxd/config.hpp>

namespace erxd {

class ConfigLoader {
public:
  static ConfigLoadResult from_file(const std::string& path);
  static ConfigLoadResult from_yaml(const std::string& document);
};

} // namespace erxd
