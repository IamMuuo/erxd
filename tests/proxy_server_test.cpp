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

TEST_CASE("proxy server accepts backend configuration") {
  erxd::ProxyServer server;

  const auto id = server.add_backend({"127.0.0.1", 9001});

  REQUIRE(id.has_value());
}
