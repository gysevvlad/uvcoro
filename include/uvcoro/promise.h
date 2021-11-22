#pragma once

#include "uvcoro/logger.h"
#include "uvcoro/operations.h"

#include <chrono>
#include <experimental/coroutine>
#include <string>
#include <utility>

#include <uv.h>

#include <iostream>

namespace uvcoro {

class unit;
class app;
struct unit_context;
class awaitable_async_unit;

namespace tcp {
class awaitable_acceptor;
class awaitable_listen;
class awaitable_write;
} // namespace tcp

class promise {
public:
  promise();

  unit get_return_object();

  template <class Rep, class Period>
  operation::delay await_transform(std::chrono::duration<Rep, Period> delay) {
    return make_delay_operation(
        std::chrono::duration_cast<std::chrono::milliseconds>(delay));
  }

  tcp::awaitable_acceptor await_transform(tcp::awaitable_acceptor aa);
  tcp::awaitable_listen await_transform(tcp::awaitable_listen al);
  tcp::awaitable_write await_transform(tcp::awaitable_write aw);
  operation::read await_transform(operation::read ro);
  operation::read_or_die await_transform(operation::read_or_die rod);

  awaitable_async_unit yield_value(unit_context unit_context);

  /**
   * uvcoro::app sets required fields on initial
   * suspend and resumes coroutine after
   */
  std::experimental::suspend_always initial_suspend();
  std::experimental::suspend_always final_suspend() noexcept;
  void unhandled_exception();
  void return_void();

  /**
   * get underlying std::experimental::coroutine_handle
   */
  std::experimental::coroutine_handle<promise> as_coroutine() {
    return std::experimental::coroutine_handle<promise>::from_promise(*this);
  }

  void set_name(std::string name) { m_name = std::move(name); }
  void set_logger(logger *logger) { m_logger = logger; }
  void set_app(app *app) { m_app = app; }
  app *get_app() { return m_app; }

  /**
   * call m_logger.trace if sets
   */
  void trace(std::string_view method_name);

  ~promise();

private:
  operation::delay make_delay_operation(std::chrono::milliseconds delay);

  app *m_app{nullptr};
  std::string m_name;
  logger *m_logger;
};

unit_context make_unit(std::string name, unit unit);

} // namespace uvcoro
