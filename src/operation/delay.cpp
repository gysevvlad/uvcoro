#include "uvcoro/operation/delay.h"

namespace uvcoro::operation {

delay::delay(uv_loop_t *loop, std::chrono::milliseconds delay)
    : m_delay(delay) {
  uv_timer_init(loop, &m_timer);
}

static void uv_timer_callback(uv_timer_t *handle) {
  coroutine_handle::from_address(handle->data).resume();
}

void delay::await_suspend(
    std::experimental::coroutine_handle<> handle) {
  m_timer.data = handle.address();
  uv_timer_start(&m_timer, uv_timer_callback, m_delay.count(), 0);
};

} // namespace uvcoro::operation
