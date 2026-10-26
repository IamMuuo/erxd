#include <erxd/config_loader.hpp>

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <sstream>
#include <utility>

namespace erxd {
namespace {

ConfigLoadResult failure(std::string error) {
  return ConfigLoadResult{std::nullopt, std::move(error)};
}

ConfigLoadResult parse(const YAML::Node& root) {
  try {
    ProxyConfig config;
    const auto listen = root["listen"];
    if (listen) {
      if (listen["host"]) {
        config.listen.host = listen["host"].as<std::string>();
      }
      if (listen["port"]) {
        config.listen.port = listen["port"].as<std::uint16_t>();
      }
    }

    const auto backends = root["backends"];
    if (!backends || !backends.IsSequence()) {
      return failure("config requires a backends sequence");
    }

    for (const auto& node : backends) {
      config.backends.push_back({node["host"].as<std::string>(),
                                 node["port"].as<std::uint16_t>()});
    }

    return ConfigLoadResult{std::move(config), {}};
  } catch (const YAML::Exception& error) {
    return failure(error.what());
  }
}

} // namespace

ConfigLoadResult ConfigLoader::from_file(const std::string& path) {
  std::ifstream file(path);
  if (!file) {
    return failure("unable to open config file: " + path);
  }

  std::stringstream document;
  document << file.rdbuf();
  return from_yaml(document.str());
}

ConfigLoadResult ConfigLoader::from_yaml(const std::string& document) {
  try {
    return parse(YAML::Load(document));
  } catch (const YAML::Exception& error) {
    return failure(error.what());
  }
}

} // namespace erxd
