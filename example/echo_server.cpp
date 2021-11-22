#include "uvcoro/uvcoro.h"
#include <chrono>

uvcoro::unit worker(uvcoro::tcp::connection connection,
                    std::chrono::seconds reply_delay) {
  static constexpr std::size_t c_default_buffer_size = 4096;
  std::array<char, c_default_buffer_size> buffer{};
  do {
    std::size_t size = co_await connection.read_in_or_die(buffer);
    co_await reply_delay;
    co_await connection.write(buffer.data(), size);
  } while (true);
}

uvcoro::unit listener(std::uint16_t port, std::chrono::seconds reply_delay) {
  auto acceptor = co_await uvcoro::tcp::make_acceptor(port);
  while (true) {
    auto connection = co_await acceptor.listen();
    co_yield uvcoro::make_unit("worker",
                               worker(std::move(connection), reply_delay));
  }
}

int main(int /*argc*/, char * /*argv*/[]) {
  static constexpr std::uint16_t c_default_port = 8088;
  static constexpr std::chrono::seconds c_default_reply_delay{5};

  uvcoro::app app("example");
  app.make_unit("hello", listener(c_default_port, c_default_reply_delay));
  return app.run();
}
