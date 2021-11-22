#include "uvcoro/operation/detail/read.h"

#include <fmt/format.h>

namespace uvcoro::operation::detail {

read::read(uv_stream_t *stream, char *data, std::size_t capacity)
    : m_stream{stream}, m_data{data}, m_capacity{capacity} {}

bool read::await_ready() { return false; }

void read::await_suspend(coroutine_handle handle) {
  m_handle = handle;
  m_stream->data = this;
  uv_read_start(m_stream, uv_alloc_callback, uv_read_callback);
}

void read::bind(uv_buf_t &buf) noexcept {
  buf.base = m_data;
  buf.len = m_capacity;
}

/* static */ void read::uv_alloc_callback(uv_handle_t *handle,
                                          size_t /*suggested_size*/,
                                          uv_buf_t *buf) noexcept {
  static_cast<read *>(handle->data)->bind(*buf);
}

void read::process_read(ssize_t nread, const uv_buf_t * /*buf*/) noexcept {
  if (nread < 0) {
    if (nread == UV_EOF) {
      m_state = state::eof;
      return;
    }
    m_state = state::err;
    m_err = static_cast<int>(nread);
    return;
  }
  m_state = state::val;
  m_size = static_cast<std::size_t>(nread);
}

void read::handle_read(ssize_t nread, const uv_buf_t *buf) noexcept {
  uv_read_stop(m_stream);
  process_read(nread, buf);
  processed();
}

/* static */ void read::uv_read_callback(uv_stream_t *stream, ssize_t nread,
                                         const uv_buf_t *buf) noexcept {
  static_cast<read *>(stream->data)->handle_read(nread, buf);
}

} // namespace uvcoro::operation::detail
