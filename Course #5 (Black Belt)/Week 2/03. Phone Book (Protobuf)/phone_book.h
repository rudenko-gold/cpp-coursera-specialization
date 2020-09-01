#pragma once

#include "iterator_range.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <iosfwd>
#include <algorithm>

#include "contact.pb.h"


struct Date {
    int year, month, day;
};

struct Contact {
    std::string name;
    std::optional<Date> birthday;
    std::vector<std::string> phones;
};

bool operator<(const Contact& lhs, const Contact& rhs) {
    return lhs.name < rhs.name;
}

bool operator<(const Contact& lhs, std::string_view str) {
    return lhs.name < str;
}

bool startWtih(const std::string& str, std::string_view prefix) {
    if (prefix.size() > str.size()) {
        return false;
    }

    for (size_t i = 0; i < prefix.size(); ++i) {
        if (str[i] != prefix[i]) {
            return false;
        }
    }
    return true;
}

class PhoneBook {
public:
    explicit PhoneBook(std::vector<Contact> contacts) : contacts(std::move(contacts)) {
        std::sort(this->contacts.begin(), this->contacts.end());
    }

    using ContactRange = IteratorRange<std::vector<Contact>::const_iterator>;

    ContactRange FindByNamePrefix(std::string_view name_prefix) const {
        if (name_prefix == "") {
            return IteratorRange(contacts.begin(), contacts.end());
        }

        auto begin = std::lower_bound(contacts.begin(), contacts.end(), name_prefix);
        auto end = begin;

        for (; end != contacts.end(); end++) {
            if (!startWtih(end->name, name_prefix)) {
                break;
            }
        }

        return IteratorRange(begin, end);
    }

    void SaveTo(std::ostream& output) const {
        PhoneBookSerialize::ContactList contactList;

        for (const auto& contact_item : contacts) {
            PhoneBookSerialize::Contact contact;

            *contact.mutable_name() = contact_item.name;

            if (contact_item.birthday.has_value()) {
                PhoneBookSerialize::Date date;

                date.set_year(contact_item.birthday->year);
                date.set_month(contact_item.birthday->month);
                date.set_day(contact_item.birthday->day);

                *contact.mutable_birthday() = date;
            }

            for (const auto& number : contact_item.phones) {
                contact.add_phone_number(number);
            }

            *contactList.add_contact() = contact;
        }

        contactList.SerializePartialToOstream(&output);
    }

private:
    std::vector<Contact> contacts;
};

PhoneBook DeserializePhoneBook(std::istream& input) {
    std::vector<Contact> contacts;

    PhoneBookSerialize::ContactList contactList;
    contactList.ParseFromIstream(&input);

    contacts.reserve(contactList.contact_size());

    for (const auto& contact : contactList.contact()) {
        Contact contact_item;

        contact_item.name = contact.name();

        if (contact.has_birthday()) {
            Date date = { contact.birthday().year(), contact.birthday().month(), contact.birthday().day() };
            contact_item.birthday = date;
        } else {
            contact_item.birthday = std::nullopt;
        }

        for (const auto& phone_number : contact.phone_number()) {
            contact_item.phones.push_back(phone_number);
        }

        contacts.emplace_back(contact_item);
    }

    return PhoneBook(contacts);
}
