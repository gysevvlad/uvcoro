#include "print.h"

#include <iostream>

namespace {

class Greeter final : public IGreeter {
public:
    void hi() const override {
        std::cout << "Hello, world!" << std::endl;
    }

    Greeter() = default;
    Greeter(const Greeter &) = delete;
    Greeter(Greeter &&) noexcept = delete;
    Greeter & operator=(const Greeter &) = delete;
    Greeter & operator=(Greeter &&) noexcept = delete;
    ~Greeter() override = default;
};

} // namespace

std::unique_ptr<IGreeter> make_greeter() {
    return std::make_unique<Greeter>();
}
