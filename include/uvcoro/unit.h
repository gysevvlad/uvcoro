#pragma once

#include "uvcoro/logger.h"

#include <experimental/coroutine>
#include <string>
#include <string_view>

namespace uvcoro {

class app;
class promise;

class unit {
public:
  using promise_type = promise;

  explicit unit(promise &promise) : m_promise{promise} {}

  promise &get_promise() { return m_promise; }

private:
  promise &m_promise;
};

struct unit_context {
  std::string name;
  unit unit;
};

class awaitable_async_unit {
public:
  explicit awaitable_async_unit(unit_context unit_context, app &app)
      : m_unit_context{std::move(unit_context)}, m_app{app} {}

  bool await_ready() { return true; }
  void await_suspend(std::experimental::coroutine_handle<> handle) {}
  void await_resume();

private:
  unit_context m_unit_context;
  app &m_app;
};

} // namespace uvcoro
