#pragma once

#include <cstddef>

namespace uvcoro::detail {

template <std::size_t Size, std::size_t Align> struct layout {
  static constexpr std::size_t size = Size;
  static constexpr std::size_t align = Align;
};

struct uv_loop_layout : layout<848, 8> {};

} // namespace uvcoro::detail
