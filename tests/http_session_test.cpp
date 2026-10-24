#include <catch2/catch_test_macros.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>

#include <erxd/backend_pool.hpp>
#include <erxd/http_session.hpp>

#include <string>

TEST_CASE("HTTP session forwards a request and response") {
  boost::asio::io_context io;
  boost::asio::ip::tcp::acceptor client_acceptor(
      io, {boost::asio::ip::tcp::v4(), 0});
  boost::asio::ip::tcp::acceptor backend_acceptor(
      io, {boost::asio::ip::tcp::v4(), 0});
  erxd::BackendPool pool;

  REQUIRE(pool.add_backend(
              {"127.0.0.1", backend_acceptor.local_endpoint().port()})
              .has_value());

  boost::asio::ip::tcp::socket client(io);
  client.connect(client_acceptor.local_endpoint());
  auto server_socket = client_acceptor.accept();

  std::string backend_request;
  backend_acceptor.async_accept(
      [&](const boost::system::error_code& error,
          boost::asio::ip::tcp::socket backend_socket) {
        REQUIRE_FALSE(error);

        auto request = std::make_shared<boost::asio::streambuf>();
        auto socket = std::make_shared<boost::asio::ip::tcp::socket>(
            std::move(backend_socket));
        boost::asio::async_read_until(
            *socket, *request, "\r\n\r\n",
            [&, request, socket](
                const boost::system::error_code& read_error, std::size_t) mutable {
              REQUIRE_FALSE(read_error);
              backend_request.assign(
                  buffers_begin(request->data()), buffers_end(request->data()));

              const std::string response =
                  "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n"
                  "Connection: close\r\n\r\nhello";
              boost::asio::async_write(
                  *socket, boost::asio::buffer(response),
                  [socket](
                      const boost::system::error_code&, std::size_t) mutable {
                    boost::system::error_code ignored;
                    socket->shutdown(
                        boost::asio::ip::tcp::socket::shutdown_both, ignored);
                    socket->close(ignored);
                  });
            });
      });

  std::make_shared<erxd::HttpSession>(std::move(server_socket), pool)->start();

  const std::string request =
      "GET /health HTTP/1.1\r\nHost: localhost\r\n\r\n";
  boost::asio::write(client, boost::asio::buffer(request));
  io.run();

  boost::asio::streambuf response;
  boost::system::error_code read_error;
  boost::asio::read(client, response, read_error);

  REQUIRE(read_error == boost::asio::error::eof);
  REQUIRE(backend_request.find("GET /health HTTP/1.1") != std::string::npos);
}
