#include <cstddef>
#include <utility>
#include <memory>

template <typename T>
struct RawMemory {
    T* buf = nullptr;
    size_t cp = 0;

    static T* Allocate(size_t n) {
        return static_cast<T*>(operator new(n * sizeof(T)));
    }

    static void Deallocate(T* buf) {
        operator delete (buf);
    }

    RawMemory() = default;

    RawMemory(size_t n) {
        buf = Allocate(n);
        cp = n;
    }

    RawMemory(const RawMemory& other) = delete;

    RawMemory(const RawMemory&& other) {
        Swap(other);
    }

    RawMemory& operator=(const RawMemory& other) = delete;

    RawMemory&operator=(RawMemory&& other) {
        Swap(other);
        return *this;
    }

    T* operator + (size_t i) {
        return buf + i;
    }

    const T* operator + (size_t i) const {
        return buf + i;
    }

    T& operator[] (size_t i) {
        return *(buf + i);
    }

    const T& operator[] (size_t i) const {
        return *(buf + i);
    }

    void Swap(RawMemory& other) {
        std::swap(other.buf, buf);
        std::swap(other.cp, cp);
    }

    ~RawMemory() {
        Deallocate(buf);
    }
};

template <typename T>
class Vector {
public:
    Vector() = default;

    Vector(size_t n) : data(n) {
        size_t i = 0;
        std::uninitialized_value_construct_n(data.buf, n);
        sz = n;
    }

    Vector(const Vector &other) : data(other.sz) {
        size_t i = 0;

        std::uninitialized_copy_n(other.data.buf, other.sz, data.buf);
        sz = other.sz;
    }

    Vector(Vector &&other) : data(other.sz) {
        Swap(other);
    }

    void Swap(Vector &other) {
        data.Swap(other.data);
        std::swap(sz, other.sz);
    }

    ~Vector() {
        std::destroy_n(data.buf, sz);
    }

    Vector &operator=(const Vector &other) {
        if (other.sz > data.cp) {
            Vector tmp(other);
            Swap(tmp);
        } else {
            for (size_t i = 0; i < sz && i < other.sz; ++i) {
                data[i] = other[i];
            }
            if (sz < other.sz) {
                std::uninitialized_copy_n(other.data.buf + sz, other.sz - sz, data.buf + sz);
            } else {
                std::destroy_n(data.buf + other.sz, sz - other.sz);
            }

            sz = other.sz;
        }

        return *this;
    }

    Vector &operator=(Vector &&other) noexcept {
        Swap(other);
        return *this;
    }

    void Reserve(size_t n) {
        if (n > data.cp) {
            RawMemory<T> data2(n);

            std::uninitialized_move_n(data.buf, sz, data2.buf);

            std::destroy_n(data.buf, sz);
            data.Swap(data2);
        }
    }

    void Resize(size_t n) {
        Reserve(n);

        if (sz < n) {
            std::uninitialized_value_construct_n(data.buf + sz, n - sz);
        } else if (sz > n) {
            std::destroy_n(data + n, sz - n);
        }

        sz = n;
    }

    void PushBack(const T &elem) {
        if (sz == data.cp) {
            Reserve(sz == 0 ? 1 : sz * 2);
        }

        new(data + sz) T(elem);
        ++sz;
    }

    void PushBack(T &&elem) {
        if (sz == data.cp) {
            Reserve(sz == 0 ? 1 : sz * 2);
        }

        new(data + sz) T(std::move(elem));
        ++sz;
    }

    template<typename ... Args>
    T &EmplaceBack(Args &&... args) {
        if (sz == data.cp) {
            Reserve(sz == 0 ? 1 : sz * 2);
        }

        new(data + sz) T(std::forward<Args>(args)...);
        return data[sz++];
    }

    void PopBack() {
        std::destroy_at(data.buf + sz - 1);
        sz--;
    }

    size_t Size() const noexcept {
        return sz;
    }

    size_t Capacity() const noexcept {
        return data.cp;
    }

    const T &operator[](size_t i) const {
        return data[i];
    }

    T &operator[](size_t i) {
        return data[i];
    }

    // Р’ РґР°РЅРЅРѕР№ С‡Р°СЃС‚Рё Р·Р°РґР°С‡Рё СЂРµР°Р»РёР·СѓР№С‚Рµ РґРѕРїРѕР»РЅРёС‚РµР»СЊРЅРѕ СЌС‚Рё С„СѓРЅРєС†РёРё:
    using iterator = T *;
    using const_iterator = const T *;

    iterator begin() noexcept {
        return data + 0;
    }

    iterator end() noexcept {
        return data + sz;
    }

    const_iterator begin() const noexcept {
        return data + 0;
    }

    const_iterator end() const noexcept {
        return data + sz;
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

    iterator Insert(const_iterator it, const T &elem) {
        const size_t pos = it - data.buf;
        RawMemory<T> data2(sz == Capacity() ? (sz == 0 ? 1 : 2 * sz) : Capacity());
        std::uninitialized_move_n(data.buf, pos, data2.buf);
        new(data2 + pos) T(elem);

        if (sz > pos) {
            std::uninitialized_move_n(data.buf + pos, sz - pos, data2.buf + pos + 1);
        }

        data.Swap(data2);
        ++sz;
        return begin() + pos;
    }

    iterator Insert(const_iterator it, T &&elem) {
        const size_t pos = it - data.buf;
        RawMemory<T> data2(sz == Capacity() ? (sz == 0 ? 1 : 2 * sz) : Capacity());
        std::uninitialized_move_n(data.buf, pos, data2.buf);
        new(data2 + pos) T(std::move(elem));

        if (sz > pos) {
            std::uninitialized_move_n(data.buf + pos, sz - pos, data2.buf + pos + 1);
        }

        data.Swap(data2);
        ++sz;
        return begin() + pos;
    }

    template<typename ... Args>
    iterator Emplace(const_iterator it, Args &&... args) {
        const size_t pos = it - data.buf;
        RawMemory<T> data2(sz == Capacity() ? (sz == 0 ? 1 : 2 * sz) : Capacity());
        std::uninitialized_move_n(data.buf, pos, data2.buf);
        new(data2 + pos) T(std::forward<Args>(args)...);

        if (sz > pos) {
            std::uninitialized_move_n(data.buf + pos, sz - pos, data2.buf + pos + 1);
        }

        data.Swap(data2);
        ++sz;
        return begin() + pos;
    }

    iterator Erase(const_iterator it) {
        const size_t pos = it - data.buf;
        RawMemory<T> data2(Capacity());
        std::uninitialized_move_n(data.buf, pos, data2.buf);
        if (sz > pos) {
            std::uninitialized_move_n(data.buf + pos + 1, sz - pos - 1, data2.buf + pos);
        }
        data.Swap(data2);
        --sz;
        return begin() + pos;
    }

private:
    static void Construct(void* buf) {
        new (buf) T;
    }

    static void Construct(void* buf, const T& elem) {
        new (buf) T(elem);
    }

    static void Destroy(T* buf) {
        buf->~T();
    }

    RawMemory<T> data;
    size_t sz = 0;
};
