#pragma once

#include "uvcoro/coroutine_handle.h"

#include <uv.h>

#include <cstddef>
#include <optional>

namespace uvcoro::operation::detail {

class read {
public:
  read(uv_stream_t *stream, char *data, std::size_t capacity);

  bool await_ready();
  void await_suspend(coroutine_handle handle);
  std::optional<std::size_t> await_resume();

  virtual void processed() = 0;

protected:
  coroutine_handle m_handle;
  enum class state { wait, eof, err, val } m_state{state::wait};
  std::size_t m_size{};
  int m_err{};

private:
  void bind(uv_buf_t &buf) noexcept;
  void process_read(ssize_t nread, const uv_buf_t *buf) noexcept;
  void handle_read(ssize_t nread, const uv_buf_t *buf) noexcept;

  static void uv_read_callback(uv_stream_t *stream, ssize_t nread,
                               const uv_buf_t *buf) noexcept;

  static void uv_alloc_callback(uv_handle_t *handle, size_t suggested_size,
                                uv_buf_t *buf) noexcept;

  uv_stream_t *m_stream;
  char *m_data;
  std::size_t m_capacity;
};

} // namespace uvcoro::operation::detail
