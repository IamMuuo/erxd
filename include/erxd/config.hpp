#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <erxd/backend_pool.hpp>

namespace erxd {

struct ListenConfig {
  std::string host{"0.0.0.0"};
  std::uint16_t port{1337};
};

struct ProxyConfig {
  ListenConfig listen;
  std::vector<BackendSpec> backends;
};

struct ConfigLoadResult {
  std::optional<ProxyConfig> config;
  std::string error;

  explicit operator bool() const { return config.has_value(); }
};

} // namespace erxd
