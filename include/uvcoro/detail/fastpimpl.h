#pragma once

#include "uvcoro/detail/layout.h"

#include <new>
#include <type_traits>
#include <utility>

namespace uvcoro::detail {

template <class layout> class fastpimpl {
public:
  template <class T> static void validate() noexcept {
    check_layot<sizeof(T), alignof(T)>();
  }

  template <class T> const T *as() const {
    return std::launder(reinterpret_cast<T *>(&m_storage));
  }

  template <class T> T *as() {
    return std::launder(reinterpret_cast<T *>(&m_storage));
  }

  template <class T, class... Args> void construct(Args &&...args) {
    new (&m_storage) T{std::forward<Args>(args)...};
  }

  template <class T> void destroy() {
    std::launder(reinterpret_cast<T *>(&m_storage))->~T();
  }

private:
  template <std::size_t ActualSize, std::size_t ActualAlign>
  static void check_layot() {
    static_assert(layout::size == ActualSize,
                  "layout::size and sizeof(T) mismatch");
    static_assert(layout::align == ActualAlign,
                  "layout::align and alignof(T) mismatch");
  }
  std::aligned_storage_t<layout::size, layout::align> m_storage;
};

} // namespace uvcoro::detail
