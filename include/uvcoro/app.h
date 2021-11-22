#pragma once

#include "uvcoro/detail/fastpimpl.h"
#include "uvcoro/logger.h"

#include <uv.h>

#include <string>
#include <vector>

namespace uvcoro {

class unit;

class app {
public:
  explicit app(std::string name);
  ~app();

  void make_unit(std::string_view name, unit && unit);
  int run();

  uv_loop_t *get_loop() { return &m_loop; }

private:
  uv_loop_t m_loop{};
  logger m_logger;
  std::string m_name;
  std::vector<unit> m_units;
};

} // namespace uvcoro
