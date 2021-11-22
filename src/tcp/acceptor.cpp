#include "uvcoro/uvcoro.h"

#include <fmt/format.h>

#include <cassert>
#include <cstring>
#include <experimental/coroutine>
#include <memory>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <variant>

namespace uvcoro::tcp {

class acceptor_impl {
public:
  acceptor_impl(uv_loop_t *loop, std::uint16_t port) {
    uv_tcp_init(loop, &m_server);
    m_server.data = this;
    uv_ip4_addr("0.0.0.0", port, &addr);
    uv_tcp_bind(&m_server, reinterpret_cast<const struct sockaddr *>(&addr), 0);
    uv_listen((uv_stream_t *)&m_server, 100, uv_connection_callback);
  }

  static void uv_connection_callback(uv_stream_t *server, int status) {
    static_cast<acceptor_impl *>(server->data)
        ->handle_connection(server, status);
  }

  void handle_connection(uv_stream_t *server, int status) {
    if (m_awaitable_listen != nullptr) {
      auto awaitable_listen = std::exchange(m_awaitable_listen, nullptr);
      awaitable_listen->wakeup(server, status);
      return;
    }
    m_queue.emplace(server, status);
  }

  std::optional<std::pair<uv_stream_t *, int>>
  request_connection(awaitable_listen *awaitable_listen) {
    assert(m_awaitable_listen == nullptr);
    if (!m_queue.empty()) {
      std::pair<uv_stream_t *, int> context = m_queue.front();
      m_queue.pop();
      return context;
    }
    m_awaitable_listen = awaitable_listen;
    return std::nullopt;
  }

private:
  uv_tcp_t m_server{};
  struct sockaddr_in addr {};
  std::queue<std::pair<uv_stream_t *, int>> m_queue;
  awaitable_listen *m_awaitable_listen;
};

class connection_impl {
public:
  connection_impl(uv_loop_t *loop, uv_stream_t *server, int status) {
    if (status < 0) {
      throw std::runtime_error(uv_strerror(status));
    }
    uv_tcp_init(loop, &m_client);
    m_client.data = this;
    auto rc = uv_accept(server, (uv_stream_t *)&m_client);
    if (rc != 0) {
      throw std::runtime_error(uv_strerror(rc));
    }
  }

  uv_stream_t *get_stream() {
    return reinterpret_cast<uv_stream_t *>(&m_client);
  }

private:
  std::vector<char> m_buffer;
  uv_tcp_t m_client{};
};

acceptor::acceptor(std::unique_ptr<acceptor_impl> impl)
    : m_impl{std::move(impl)} {}

acceptor::~acceptor() = default;

acceptor awaitable_acceptor::await_resume() {
  return acceptor{std::make_unique<acceptor_impl>(
      m_promise->get_app()->get_loop(), m_port)};
}

void awaitable_listen::wakeup(uv_stream_t *server, int status) {
  m_result.emplace(server, status);
  m_handle.resume();
}

bool awaitable_listen::await_ready() {
  m_result = m_acceptor_impl.request_connection(this);
  return m_result.has_value();
}

void awaitable_listen::await_suspend(
    std::experimental::coroutine_handle<> handle) {
  m_handle = handle;
}

connection awaitable_listen::await_resume() {
  return connection{std::make_unique<connection_impl>(
      m_promise->get_app()->get_loop(), m_result->first, m_result->second)};
}

connection::connection(std::unique_ptr<connection_impl> impl)
    : m_impl{std::move(impl)} {}

connection::connection(connection &&other) noexcept
    : m_impl{std::move(other.m_impl)} {}

connection &connection::operator=(connection &&other) noexcept {
  m_impl = std::move(other.m_impl);
  return *this;
}

connection::~connection() = default;

operation::read connection::read_in_impl(char *data, std::size_t size) {
  return {m_impl->get_stream(), data, size};
}

operation::read_or_die connection::read_in_or_die_impl(char *data,
                                                       std::size_t size) {
  return {m_impl->get_stream(), data, size};
}

bool awaitable_write::await_ready() { return false; }

static void uv_write_callback(uv_write_t *req, int status) {
  static_cast<awaitable_write *>(req->data)->wakeup(status);
}

void awaitable_write::wakeup(int status) { m_handle.resume(); }

void awaitable_write::await_suspend(
    std::experimental::coroutine_handle<> handle) {
  m_handle = handle;
  m_write.data = this;
  uv_write(&m_write, m_connection_impl.get_stream(), &m_buf, 1,
           uv_write_callback);
}

void awaitable_write::await_resume() {}

awaitable_write connection::write(char *data, std::size_t size) {
  return awaitable_write{*m_impl, data, size};
}

} // namespace uvcoro::tcp
