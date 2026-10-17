#include <catch2/catch_test_macros.hpp>
#include <erxd/backend_pool.hpp>
#include <memory>

TEST_CASE("Backend pool initializes empty") {
  const auto pool = std::make_unique<erxd::BackendPool>();

  REQUIRE_FALSE(pool->next().has_value());
}

TEST_CASE("Backend pool assigns IDs and returns endpoints") {
  erxd::BackendPool pool;

  const auto id = pool.add_backend({"127.0.0.1", 9001});

  REQUIRE(id.has_value());
  REQUIRE(*id == 1);

  const auto endpoint = pool.next();
  REQUIRE(endpoint.has_value());
  REQUIRE(endpoint->id == *id);
  REQUIRE(endpoint->host == "127.0.0.1");
  REQUIRE(endpoint->port == 9001);
}

TEST_CASE("Backend pool skips removed backends") {
  erxd::BackendPool pool;

  const auto first = pool.add_backend({"127.0.0.1", 9001});
  const auto second = pool.add_backend({"127.0.0.1", 9002});

  REQUIRE(first.has_value());
  REQUIRE(second.has_value());
  REQUIRE(pool.remove_backend(*first));

  const auto endpoint = pool.next();
  REQUIRE(endpoint.has_value());
  REQUIRE(endpoint->id == *second);
}

TEST_CASE("Backend pool does not reuse IDs") {
  erxd::BackendPool pool;

  const auto first = pool.add_backend({"127.0.0.1", 9001});
  REQUIRE(first.has_value());
  REQUIRE(pool.remove_backend(*first));

  const auto second = pool.add_backend({"127.0.0.1", 9002});
  REQUIRE(second.has_value());
  REQUIRE(*second > *first);
}
