#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>

#include <cstdint>

namespace erxd {

class IProxyServer {
public:
  virtual ~IProxyServer() = default;

  virtual std::uint16_t port() const = 0;
  virtual void run() = 0;
};

class ProxyServer final : public IProxyServer {
public:
  explicit ProxyServer(std::uint16_t port = 1337);

  std::uint16_t port() const override;
  void run() override;

private:
  void accept_connection();

  boost::asio::io_context io_context_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::uint16_t port_;
};

} // namespace erxd
