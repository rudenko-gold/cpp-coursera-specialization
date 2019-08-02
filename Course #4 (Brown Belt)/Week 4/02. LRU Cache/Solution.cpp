#include "Common.h"
#include <map>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <algorithm>

using namespace std;

class LruCache : public ICache {
private:
    struct Entry {
        BookPtr ptr;
        int rank = 0;
    };
public:
    LruCache(
            shared_ptr<IBooksUnpacker> books_unpacker,
            const Settings& settings
    ) : unpacker(move(books_unpacker)), settings(settings) {}

    BookPtr GetBook(const string& book_name) override {
        lock_guard<mutex> guard(mute);

        if (auto it = data.find(book_name); it != data.end()) {
            it->second.rank = ++maxRank;
            return it->second.ptr;
        } else {
            shared_ptr<IBook> book = unpacker->UnpackBook(book_name);
            AddEntry(book);
            ShrinkToFit();
            return move(book);
        }
    }

    void DeleteLastEntry() {
        if (data.empty()) {
            return;
        }

        auto it = min_element(data.begin(), data.end(), [](pair<string, Entry> lhs, pair<string, Entry> rhs) {
            return lhs.second.rank < rhs.second.rank;
        });

        usedMemory -= it->second.ptr->GetContent().size();
        data.erase(it);
    }

    void AddEntry(shared_ptr<IBook> book) {
        usedMemory += book->GetContent().size();
        string book_name = book->GetName();
        maxRank++;
        data[book_name] = { move(book), maxRank };
    }

    void ShrinkToFit() {
        while (usedMemory > settings.max_memory) {
            DeleteLastEntry();
        }
    }

private:
    mutex mute;
    int maxRank = 0;
    unordered_map<string, Entry> data;
    size_t usedMemory = 0;
    const Settings settings;
    shared_ptr<IBooksUnpacker> unpacker;
};

unique_ptr<ICache> MakeCache(
        shared_ptr<IBooksUnpacker> books_unpacker,
        const ICache::Settings& settings
) {
    return make_unique<LruCache>(books_unpacker, settings);
}
