#pragma once

#include <array>
#include <cstdint>
#include <experimental/coroutine>
#include <memory>
#include <optional>

#include <uv.h>

#include "uvcoro/operations.h"

namespace uvcoro {
class promise;

} // namespace uvcoro

namespace uvcoro::tcp {

class acceptor;
class acceptor_impl;
class connection;
class connection_impl;
class awaitable_write;

class awaitable_write {
public:
  explicit awaitable_write(connection_impl &connection_impl, char *data,
                           std::size_t size)
      : m_connection_impl{connection_impl} {
    m_buf.base = data;
    m_buf.len = size;
  }

  bool await_ready();
  void await_suspend(std::experimental::coroutine_handle<> handle);
  void await_resume();

  void wakeup(int status);
  void set_promise(promise *promise) { m_promise = promise; }

private:
  connection_impl &m_connection_impl;
  promise *m_promise{nullptr};
  std::experimental::coroutine_handle<> m_handle;
  uv_write_t m_write{};
  uv_buf_t m_buf;
};

class awaitable_listen {
public:
  explicit awaitable_listen(acceptor_impl &acceptor_impl)
      : m_acceptor_impl{acceptor_impl} {}

  awaitable_listen(awaitable_listen &&) noexcept = default;
  ~awaitable_listen() = default;

  void wakeup(uv_stream_t *server, int status);

  bool await_ready();
  void await_suspend(std::experimental::coroutine_handle<> handle);
  connection await_resume();

  void ser_promise(promise *promise) { m_promise = promise; }

  awaitable_listen(const awaitable_listen &) = delete;
  awaitable_listen &operator=(const awaitable_listen &) = delete;
  awaitable_listen &operator=(awaitable_listen &&) noexcept = delete;

private:
  acceptor_impl &m_acceptor_impl;
  promise *m_promise{nullptr};
  std::experimental::coroutine_handle<> m_handle;
  std::optional<std::pair<uv_stream_t *, int>> m_result;
};

class connection {
public:
  explicit connection(std::unique_ptr<connection_impl> connection_impl);
  connection(const connection &) = delete;
  connection(connection &&) noexcept;
  connection &operator=(const connection &) = delete;
  connection &operator=(connection &&) noexcept;
  ~connection();

  template <std::size_t SIZE>
  operation::read read_in(std::array<char, SIZE> &data) {
    return read_in_impl(data.data(), SIZE);
  }

  template <std::size_t SIZE>
  operation::read_or_die read_in_or_die(std::array<char, SIZE> &data) {
    return read_in_or_die_impl(data.data(), SIZE);
  }


  awaitable_write write(char *data, std::size_t size);

private:
  operation::read read_in_impl(char *data, std::size_t size);
  operation::read_or_die read_in_or_die_impl(char *data, std::size_t size);
  std::unique_ptr<connection_impl> m_impl;
};

class awaitable_acceptor {
public:
  awaitable_acceptor() = default;

  bool await_ready() { return true; }
  void await_suspend(std::experimental::coroutine_handle<> handle) {}
  acceptor await_resume();

  void set_port(std::uint16_t port) { m_port = port; }
  void set_promise(promise *promise) { m_promise = promise; }

private:
  std::uint16_t m_port;
  promise *m_promise{nullptr};
};

class acceptor {
public:
  explicit acceptor(std::unique_ptr<acceptor_impl> impl);
  acceptor(const acceptor &) = delete;
  acceptor(acceptor &&) noexcept = default;
  acceptor &operator=(const acceptor &) = delete;
  acceptor &operator=(acceptor &&) noexcept = default;
  ~acceptor();

  awaitable_listen listen() { return awaitable_listen{*m_impl}; }

private:
  std::unique_ptr<acceptor_impl> m_impl;
};

static awaitable_acceptor make_acceptor(std::uint16_t port) {
  awaitable_acceptor aa;
  aa.set_port(port);
  return aa;
}

} // namespace uvcoro::tcp
