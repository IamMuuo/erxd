#include <catch2/catch_test_macros.hpp>

#include <erxd/proxy_server.hpp>

TEST_CASE("proxy server uses the default port") {
  const erxd::ProxyServer server;

  REQUIRE(server.port() == 1337);
}

TEST_CASE("proxy server accepts a configured port") {
  const erxd::ProxyServer server(8080);

  REQUIRE(server.port() == 8080);
}
