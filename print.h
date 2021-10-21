#pragma once

#include <memory>

class IGreeter {
public:
    virtual void hi() const = 0;

    IGreeter() = default;
    IGreeter(const IGreeter &) = delete;
    IGreeter & operator=(const IGreeter &) = delete;
    IGreeter(IGreeter &&) noexcept = delete;
    IGreeter & operator=(IGreeter &&) noexcept = delete;
    virtual ~IGreeter() = default;
};

std::unique_ptr<IGreeter> make_greeter();
