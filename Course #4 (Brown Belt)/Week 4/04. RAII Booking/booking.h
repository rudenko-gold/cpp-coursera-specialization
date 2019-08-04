#include <utility>

namespace RAII {
    template <typename Provider>
    class Booking {
    public:
        Booking(Provider* provider, int counter) : provider(std::move(provider)), booking_id(counter) {}

        Booking(const Booking&) = delete;

        Booking& operator=(const Booking&) = delete;

        Booking(Booking&& other) noexcept {
            provider = std::move(other.provider);
            booking_id = std::move(other.booking_id);
        };

        Booking& operator= (Booking&& other) noexcept  {
            provider = std::move(other.provider);
            other.provider = nullptr;
            booking_id = std::move(other.booking_id);
            return *this;
        }

        ~Booking() {
            if (provider) {
                provider->CancelOrComplete(*this);
            }
        }
    private:
        Provider* provider;
        int booking_id;
    };
}
