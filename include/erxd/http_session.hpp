#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

#include <array>
#include <memory>

#include <erxd/backend_pool.hpp>

namespace erxd {

class HttpSession final : public std::enable_shared_from_this<HttpSession> {
public:
  HttpSession(boost::asio::ip::tcp::socket client, IBackendPool& backend_pool);

  void start();

private:
  void read_request();
  void connect_backend(BackendEndpoint endpoint);
  void write_request();
  void read_response();
  void write_response();
  [[nodiscard]] bool close();

  boost::asio::ip::tcp::socket client_;
  boost::asio::ip::tcp::socket backend_;
  boost::asio::ip::tcp::resolver resolver_;
  IBackendPool& backend_pool_;
  boost::asio::streambuf request_;
  boost::asio::streambuf response_;
};

} // namespace erxd
