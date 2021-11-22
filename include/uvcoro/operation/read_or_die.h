#pragma once

#include "uvcoro/operation/detail/read.h"

namespace uvcoro::operation {

class read_or_die : public detail::read {
public:
  read_or_die(uv_stream_t *stream, char *data, std::size_t capacity)
      : detail::read{stream, data, capacity} {}

  std::size_t await_resume();

  void processed() override;
};

} // namespace uvcoro::operation
