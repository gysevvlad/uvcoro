#pragma once

#include "uvcoro/operation/detail/read.h"

#include <optional>

namespace uvcoro::operation {

class read : public detail::read {
public:
  read(uv_stream_t *stream, char *data, std::size_t capacity)
      : detail::read{stream, data, capacity} {}

  std::optional<std::size_t> await_resume();

  void processed() override;
};

} // namespace uvcoro::operation
