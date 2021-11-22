#include "uvcoro/uvcoro.h"

#include "uv.h"
#include <experimental/coroutine>
#include <fmt/format.h>
#include <utility>

namespace uvcoro {

app::app(std::string name) : m_name{std::move(name)} {
  m_logger.trace = [](std::string_view text) {
    std::clog << text << std::endl;
  };
  m_logger.trace(fmt::format("[{}] {}", m_name, __PRETTY_FUNCTION__));
  uv_loop_init(&m_loop);
}

void app::make_unit(std::string_view name, unit &&unit) {
  unit.get_promise().set_app(this);
  unit.get_promise().set_name(fmt::format("{}.{}", m_name, name));
  unit.get_promise().set_logger(&m_logger);
  unit.get_promise().as_coroutine().resume();
}

int app::run() { return uv_run(&m_loop, UV_RUN_DEFAULT); }

app::~app() {
  for (auto &unit : m_units) {
    unit.get_promise().as_coroutine().destroy();
  }
}

} // namespace uvcoro
