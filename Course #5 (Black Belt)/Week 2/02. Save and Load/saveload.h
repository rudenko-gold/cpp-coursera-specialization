#pragma once
#include <map>
#include <iostream>
#include <string>
#include <vector>


// Serialization
template <typename T>
void Serialize(T pod, std::ostream& out);

void Serialize(const std::string& str, std::ostream& out);

template <typename T>
void Serialize(const std::vector<T>& data, std::ostream& out);

template <typename T1, typename T2>
void Serialize(const std::map<T1, T2>& data, std::ostream& out);


template <typename T>
void Serialize(T pod, std::ostream& out) {
    out.write(reinterpret_cast<const char*>(&pod), sizeof(pod));
}

inline void Serialize(const std::string& str, std::ostream& out) {
    size_t size = str.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (const auto& ch : str) {
        Serialize(ch, out);
    }
}

template <typename T>
void Serialize(const std::vector<T>& data, std::ostream& out) {
    size_t size = data.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (const auto& item : data) {
        Serialize(item, out);
    }
}

template <typename T1, typename T2>
void Serialize(const std::map<T1, T2>& data, std::ostream& out) {
    size_t size = data.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (const auto& [key, value] : data) {
        Serialize(key, out);
        Serialize(value, out);
    }
}

// Deserialization
template <typename T>
void Deserialize(std::istream& in, T& pod);

void Deserialize(std::istream& in, std::string& str);

template <typename T>
void Deserialize(std::istream& in, std::vector<T>& data);

template <typename T1, typename T2>
void Deserialize(std::istream& in, std::map<T1, T2>& data);

template <typename T>
void Deserialize(std::istream& in, T& pod) {
    in.read(reinterpret_cast<char*>(&pod), sizeof(pod));
}

inline void Deserialize(std::istream& in, std::string& str) {
    size_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    str = std::string(size, ' ');
    for (auto& ch : str) {
        Deserialize(in, ch);
    }
}


template <typename T>
void Deserialize(std::istream& in, std::vector<T>& data) {
    size_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    data = std::vector<T>(size);
    for (auto& item : data) {
        Deserialize(in, item);
    }
}


template <typename T1, typename T2>
void Deserialize(std::istream& in, std::map<T1, T2>& data) {
    size_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    for (size_t i = 0; i < size; ++i) {
        T1 key;
        T2 value;
        Deserialize(in, key);
        Deserialize(in, value);
        data[key] = value;
    }
}
