#include "uvcoro/uvcoro.h"

#include "fmt/format.h"
#include <chrono>
#include <utility>

namespace uvcoro {

promise::promise() = default;

std::experimental::suspend_always promise::initial_suspend() { return {}; }

unit promise::get_return_object() { return unit{*this}; }

std::experimental::suspend_always promise::final_suspend() noexcept {
  trace(__PRETTY_FUNCTION__);
  return {};
}

void promise::unhandled_exception() { trace(__PRETTY_FUNCTION__); }

void promise::trace(std::string_view method_name) {
  if (m_logger->trace) {
    m_logger->trace(fmt::format("[{}] {}", m_name, method_name));
  }
}

void promise::return_void() { trace(__PRETTY_FUNCTION__); }

promise::~promise() { trace(__PRETTY_FUNCTION__); }

operation::delay
promise::make_delay_operation(std::chrono::milliseconds delay) {
  trace(__PRETTY_FUNCTION__);
  return {m_app->get_loop(), delay};
}

tcp::awaitable_acceptor
promise::await_transform(tcp::awaitable_acceptor awaitable_acceptor) {
  trace(__PRETTY_FUNCTION__);
  awaitable_acceptor.set_promise(this);
  return awaitable_acceptor;
}

tcp::awaitable_listen promise::await_transform(tcp::awaitable_listen al) {
  trace(__PRETTY_FUNCTION__);
  al.ser_promise(this);
  return al;
}

tcp::awaitable_write promise::await_transform(tcp::awaitable_write aw) {
  trace(__PRETTY_FUNCTION__);
  aw.set_promise(this);
  return aw;
}

operation::read promise::await_transform(operation::read ro) {
  trace(__PRETTY_FUNCTION__);
  return ro;
}

operation::read_or_die promise::await_transform(operation::read_or_die rod) {
  trace(__PRETTY_FUNCTION__);
  return rod;
}

unit_context make_unit(std::string name, unit unit) {
  return {std::move(name), std::move(unit)};
}

awaitable_async_unit promise::yield_value(unit_context unit_context) {
  return awaitable_async_unit{std::move(unit_context), *m_app};
}

} // namespace uvcoro
