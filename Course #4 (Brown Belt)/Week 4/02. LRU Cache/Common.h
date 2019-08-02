#pragma once

#include <memory>
#include <string>

class IBook {
public:
    virtual ~IBook() = default;

    virtual const std::string& GetName() const = 0;

    virtual const std::string& GetContent() const = 0;
};

class IBooksUnpacker {
public:
    virtual ~IBooksUnpacker() = default;

    virtual std::unique_ptr<IBook> UnpackBook(const std::string& book_name) = 0;
};

class ICache {
public:
    struct Settings {
        size_t max_memory = 0;
    };

    using BookPtr = std::shared_ptr<const IBook>;

public:
    virtual ~ICache() = default;
    virtual BookPtr GetBook(const std::string& book_name) = 0;
};

std::unique_ptr<ICache> MakeCache(
        std::shared_ptr<IBooksUnpacker> books_unpacker,
        const ICache::Settings& settings
);
