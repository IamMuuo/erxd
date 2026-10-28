#include <erxd/config_loader.hpp>
#include <erxd/proxy_server.hpp>

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  const std::string config_path =
      argc > 1 ? argv[1] : "config/proxy.yaml";
  const auto loaded = erxd::ConfigLoader::from_file(config_path);

  if (!loaded) {
    std::cerr << "configuration error: " << loaded.error << '\n';
    return 1;
  }

  erxd::ProxyServer server(loaded.config->listen.port);
  for (const auto& backend : loaded.config->backends) {
    if (!server.add_backend(backend)) {
      std::cerr << "configuration error: unable to add backend\n";
      return 1;
    }
  }

  server.run();
}
