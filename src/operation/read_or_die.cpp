#include "uvcoro/operation/read_or_die.h"

#include <fmt/format.h>

#include <exception>

namespace uvcoro::operation {

void read_or_die::processed() {
  m_state == state::eof ? m_handle.destroy() : m_handle.resume();
}

std::size_t read_or_die::await_resume() {
  switch (m_state) {
  case state::val:
    return m_size;
  case state::err:
    throw std::runtime_error(fmt::format("{}: [{}({}): {}", "uv_read_cb",
                                         uv_err_name(m_err), m_err,
                                         uv_strerror(m_err)));
  case state::wait:
    [[fallthrough]];
  case state::eof:
    // unexpected branch
    std::terminate();
  }
}

} // namespace uvcoro::operation
