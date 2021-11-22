#include "uvcoro/uvcoro.h"

#include "fmt/format.h"

namespace uvcoro {

void awaitable_async_unit::await_resume() {
  m_app.make_unit(std::move(m_unit_context.name),
                  std::move(m_unit_context.unit));
}

} // namespace uvcoro
