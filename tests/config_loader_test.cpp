#include <catch2/catch_test_macros.hpp>

#include <erxd/config_loader.hpp>

TEST_CASE("config loader reads listen and backend settings") {
  const auto result = erxd::ConfigLoader::from_yaml(
      "listen:\n"
      "  host: 127.0.0.1\n"
      "  port: 8080\n"
      "backends:\n"
      "  - host: 127.0.0.1\n"
      "    port: 9001\n"
      "  - host: 127.0.0.1\n"
      "    port: 9002\n");

  REQUIRE(result);
  REQUIRE(result.config->listen.host == "127.0.0.1");
  REQUIRE(result.config->listen.port == 8080);
  REQUIRE(result.config->backends.size() == 2);
  REQUIRE(result.config->backends[1].port == 9002);
}

TEST_CASE("config loader returns an error for invalid configuration") {
  const auto result = erxd::ConfigLoader::from_yaml("listen: {}\n");

  REQUIRE_FALSE(result);
  REQUIRE_FALSE(result.error.empty());
}
