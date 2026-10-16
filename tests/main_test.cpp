#include <catch2/catch_test_macros.hpp>

#include <boost/asio/io_context.hpp>

TEST_CASE("Boost.Asio is available") {
  boost::asio::io_context io;

  REQUIRE(io.stopped() == false);
}
