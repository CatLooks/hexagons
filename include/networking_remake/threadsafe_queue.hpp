#pragma once

#include <queue>
#include <mutex>
#include <optional>

template<typename T>
class ThreadSafeQueue {
public:
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(value);
    }

    std::optional<T> pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return std::nullopt;
        }
        T val = m_queue.front();
        m_queue.pop();
        return val;
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
};