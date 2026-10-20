#include "erxd/proxy_server.hpp"

#include <erxd/http_session.hpp>

namespace erxd {

ProxyServer::ProxyServer(std::uint16_t port)
    : io_context_{}, acceptor_{io_context_,
                               boost::asio::ip::tcp::endpoint{
                                   boost::asio::ip::tcp::v4(), port}},
      port_{port} {
  accept_connection();
}

std::uint16_t ProxyServer::port() const { return port_; }

void ProxyServer::accept_connection() {
  acceptor_.async_accept([this](const boost::system::error_code &error,
                                boost::asio::ip::tcp::socket socket) {
    if (!error) {
      std::make_shared<HttpSession>(std::move(socket), backend_pool_)->start();
    }

    if (acceptor_.is_open()) {
      accept_connection();
    }
  });
}

void ProxyServer::run() { io_context_.run(); }

} // namespace erxd
