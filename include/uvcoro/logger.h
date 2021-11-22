#pragma once

#include <string_view>

namespace uvcoro {

struct logger {
  std::function<void(std::string_view)> trace;
};

} // namespace uvcoro
