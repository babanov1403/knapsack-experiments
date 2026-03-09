#pragma once

#include <bits/stdc++.h>

constexpr std::size_t kBarWidth = 50;

class BabanovTqdm {
    template <std::size_t Steps = 1000, bool OutputOn = true>
    class Iterator {
    public:
        Iterator() = delete;
        explicit Iterator(std::int64_t value, std::int64_t max_steps, BabanovTqdm* father) noexcept
            : value_(value)
            , max_steps_(max_steps)
            , bar_(father->bar_) {}

        constexpr Iterator& operator++() {
            ++value_;
            if constexpr (OutputOn) {
                if (value_ % Steps == 0) {
                    display();
                }
            }
            return *this;
        }

        constexpr std::int64_t& operator*() noexcept {
            return value_;
        }

        constexpr std::int64_t operator*() const noexcept {
            return value_;
        }

        constexpr bool operator==(const Iterator& other) {
            return value_ == other.value_;
        }

        constexpr bool operator!=(const Iterator& other) {
            return value_ != other.value_;
        }
    private:
        void display() {
            double percentage = (value_ == max_steps_) ? 100.0 : (100.0 * value_ / max_steps_);
            std::size_t filled = static_cast<std::size_t>(percentage * kBarWidth / 100.0);
            std::fill_n(bar_.begin(), filled, '=');
            std::fill(bar_.begin() + filled, bar_.end(), '-');
            std::cout << '\r' << std::fixed << std::setprecision(1)
                  << std::setw(5) << percentage << "% ▕"
                  << bar_ << "▏ " << value_ << '/' << max_steps_
                  << std::flush;
        }
    private:
        std::int64_t value_;
        std::int64_t max_steps_;
        std::string& bar_;
    };
public:
    constexpr explicit BabanovTqdm(std::int64_t to) noexcept : from_(0), to_(to) {
        bar_.resize(kBarWidth);
    }

    Iterator<> begin() {
        return Iterator<>(from_, to_, this);
    }

    Iterator<> end() {
        return Iterator<>(to_, to_, this);
    }
private:
    std::int64_t from_;
    std::int64_t to_;
    std::string bar_;
};