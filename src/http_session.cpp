#include <erxd/http_session.hpp>

#include <boost/asio/async_result.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/log/trivial.hpp>

#include <algorithm>
#include <string>
#include <utility>

namespace {

std::string request_line(const boost::asio::streambuf &request) {
  const auto data = request.data();
  const auto begin = boost::asio::buffers_begin(data);
  const auto end = boost::asio::buffers_end(data);
  const auto line_end = std::search(begin, end, "\r\n", "\r\n" + 2);
  return std::string(begin, line_end);
}

} // namespace

namespace erxd {

HttpSession::HttpSession(boost::asio::ip::tcp::socket client,
                         IBackendPool &backend_pool)
    : client_{std::move(client)}, backend_{client_.get_executor()},
      resolver_{client_.get_executor()}, backend_pool_{backend_pool} {}

void HttpSession::start() { read_request(); }

void HttpSession::read_request() {
  auto self = shared_from_this();
  boost::asio::async_read_until(
      client_, request_, "\r\n\r\n",
      [self](const boost::system::error_code &error, std::size_t) {
        if (error) {
          (void)self->close();
          return;
        }

        const auto backend = self->backend_pool_.next();
        if (!backend) {
          (void)self->close();
          return;
        }

        BOOST_LOG_TRIVIAL(info)
            << "request " << request_line(self->request_) << " -> backend "
            << backend->host << ':' << backend->port;
        self->connect_backend(*backend);
      });
}

void HttpSession::connect_backend(BackendEndpoint endpoint) {
  auto self = shared_from_this();
  resolver_.async_resolve(
      endpoint.host, std::to_string(endpoint.port),
      [self](const boost::system::error_code &error,
             boost::asio::ip::tcp::resolver::results_type results) {
        if (error) {
          (void)self->close();
          return;
        }

        boost::asio::async_connect(
            self->backend_, results,
            [self](const boost::system::error_code &connect_error,
                   const boost::asio::ip::tcp::endpoint &) {
              if (connect_error) {
                (void)self->close();
                return;
              }

              self->write_request();
            });
      });
}

void HttpSession::write_request() {
  auto self = shared_from_this();
  boost::asio::async_write(
      backend_, request_,
      [self](const boost::system::error_code &error, std::size_t) {
        if (error) {
          (void)self->close();
          return;
        }

        self->read_response();
      });
}

void HttpSession::read_response() {
  auto self = shared_from_this();
  backend_.async_read_some(
      response_.prepare(8192),
      [self](const boost::system::error_code &error, std::size_t bytes) {
        if (!error) {
          self->response_.commit(bytes);
          self->read_response();
          return;
        }

        if (error != boost::asio::error::eof) {
          (void)self->close();
          return;
        }

        self->write_response();
      });
}

void HttpSession::write_response() {
  auto self = shared_from_this();
  boost::asio::async_write(client_, response_,
                           [self](const boost::system::error_code &,
                                  std::size_t) { (void)self->close(); });
}

bool HttpSession::close() {
  bool success = true;
  boost::system::error_code error;

  client_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
  if (error && error != boost::asio::error::not_connected) {
    success = false;
  }

  error.clear();
  client_.close(error);
  if (error) {
    success = false;
  }

  error.clear();
  backend_.close(error);
  if (error) {
    success = false;
  }

  resolver_.cancel();
  return success;
}

} // namespace erxd
