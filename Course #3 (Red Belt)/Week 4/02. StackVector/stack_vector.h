#include <array>
#include <exception>
#include <system_error>
#include <stdexcept>

template <typename T, size_t capacity>
class StackVector {
public:
    explicit StackVector(size_t a_size = 0) {
        if (a_size > capacity) {
            throw std::invalid_argument("");
        } else {
            data_size = a_size;
        }
    }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    auto begin() {
        return data.begin();
    }

    auto end() {
        return data.begin() + data_size;
    }

    auto begin() const {
        return data.begin();
    }

    auto end() const {
        return data.begin() + data_size;
    }

    size_t Size() const {
        return data_size;
    }

    size_t Capacity() const {
        return data.size();
    }

    void PushBack(const T& value) {
        if (data_size < capacity) {
            data[data_size] = value;
            data_size++;
        } else {
            throw std::overflow_error("");
        }
    }

    T PopBack() {
        if (data_size <= 0) {
            throw std::underflow_error("");
        } else {
            T ret = data[data_size - 1];
            data_size--;
            return ret;
        }
    }

    size_t data_size;

    std::array<T, capacity> data;
};
