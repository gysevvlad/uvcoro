#pragma once

#include "uvcoro/coroutine_handle.h"

#include <uv.h>

#include <chrono>

namespace uvcoro::operation {

class delay {
public:
  delay(uv_loop_t * loop, std::chrono::milliseconds delay);

  bool await_ready() { return false; }
  void await_suspend(coroutine_handle handle);
  void await_resume() {}

private:
  std::chrono::milliseconds m_delay;
  uv_timer_t m_timer{};
};

} // namespace uvcoro::operation
