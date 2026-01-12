#pragma once
#include <functional>
#include <vector>
#include <map>

// A simple delegate/event system for broadcasting events to multiple listeners.
template<typename... Args>
class Delegate {
public:
    // Type for a function that can be bound
    using TFunc = std::function<void(Args...)>;

    // Add a function to the list of listeners
    void Add(const TFunc& function) {
        m_Listeners.push_back(function);
    }

    // Broadcast the event to all listeners
    void Broadcast(Args... args) {
        for (const auto& listener : m_Listeners) {
            if (listener) {
                listener(args...);
            }
        }
    }

private:
    std::vector<TFunc> m_Listeners;
};