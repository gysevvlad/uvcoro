#include "uvcoro/operation/read.h"

#include <fmt/format.h>

#include <exception>

namespace uvcoro::operation {

void read::processed() { m_handle.resume(); }

std::optional<std::size_t> read::await_resume() {
  switch (m_state) {
  case state::val:
    return m_size;
  case state::eof:
    return std::nullopt;
  case state::err:
    throw std::runtime_error(fmt::format("{}: [{}({}): {}", "uv_read_cb",
                                         uv_err_name(m_err), m_err,
                                         uv_strerror(m_err)));
  case state::wait:
    // unexpected branch
    std::terminate();
  }
}

} // namespace uvcoro::operation
