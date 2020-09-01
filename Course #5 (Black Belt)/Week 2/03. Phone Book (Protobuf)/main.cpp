#include "phone_book.h"
#include "contact.pb.h"

#include "test_runner.h"

#include <sstream>

using namespace std;

void TestSerialization() {
    const PhoneBook ab({
                               {"Ivan Ivanov", Date{1980, 1, 13}, {"+79850685521"}},
                               {"Margarita Petrova", Date{1989, 4, 23}, {"+79998887766", "+71112223344"}},
                               {"Just Birthday", Date{1989, 4, 23}, {}},
                               {"No Birthday", std::nullopt, {"+7-4862-77-25-64"}},
                       });

    ostringstream output(std::ios::binary);
    ab.SaveTo(output);

    istringstream input(output.str(), std::ios::binary);
    PhoneBookSerialize::ContactList list;
    ASSERT(list.ParseFromIstream(&input));
    ASSERT_EQUAL(list.contact_size(), 4);

    unordered_map<string, const PhoneBookSerialize::Contact*> persons;
    for (const auto& p : list.contact()) {
        persons[p.name()] = &p;
    }

    {
        auto it = persons.find("Ivan Ivanov");
        ASSERT(it != persons.end());
        const auto& p = *it->second;

        ASSERT(p.has_birthday());
        ASSERT_EQUAL(p.birthday().year(), 1980);
    }
    {
        auto it = persons.find("Margarita Petrova");
        ASSERT(it != persons.end());
        const auto& p = *it->second;

        ASSERT(p.has_birthday());
        ASSERT_EQUAL(p.birthday().year(), 1989);
        ASSERT_EQUAL(p.phone_number_size(), 2);
        ASSERT_EQUAL(p.phone_number(1), "+71112223344");
    }
    {
        auto it = persons.find("Just Birthday");
        ASSERT(it != persons.end());
        const auto& p = *it->second;

        ASSERT(p.has_birthday());
        ASSERT_EQUAL(p.birthday().day(), 23);
        ASSERT_EQUAL(p.phone_number_size(), 0);
    }
    {
        auto it = persons.find("No Birthday");
        ASSERT(it != persons.end());
        const auto& p = *it->second;

        ASSERT(!p.has_birthday());
        ASSERT_EQUAL(p.phone_number_size(), 1);
    }
}

void TestDeserialization() {
    PhoneBookSerialize::ContactList list;

    PhoneBookSerialize::Contact p;
    p.set_name("Vanya");
    p.add_phone_number("+7-953-181-46-12");

    *list.add_contact() = p;

    p.Clear();
    p.set_name("Veronika");
    p.mutable_birthday()->set_year(2000);
    p.mutable_birthday()->set_month(3);
    p.mutable_birthday()->set_day(31);
    p.add_phone_number("325-87-16");

    *list.add_contact() = p;

    p.Clear();
    p.set_name("Vitya");

    *list.add_contact() = p;

    ostringstream output(std::ios::binary);
    list.SerializeToOstream(&output);

    istringstream input(output.str(), std::ios::binary);
    const PhoneBook book = DeserializePhoneBook(input);

    auto range = book.FindByNamePrefix("");
    vector<Contact> contacts(range.begin(), range.end());

    ASSERT_EQUAL(contacts.size(), 3);

    ASSERT_EQUAL(contacts[0].name, "Vanya");
    ASSERT_EQUAL(contacts[0].phones, (vector<string>{"+7-953-181-46-12"}));
    ASSERT(!contacts[0].birthday);

    ASSERT_EQUAL(contacts[1].name, "Veronika");
    ASSERT(contacts[1].birthday);
    ASSERT_EQUAL(contacts[1].birthday->year, 2000);
    ASSERT_EQUAL(contacts[1].birthday->month, 3);
    ASSERT_EQUAL(contacts[1].birthday->day, 31);
    ASSERT_EQUAL(contacts[1].phones, (vector<string>{"325-87-16"}));

    ASSERT_EQUAL(contacts[2].name, "Vitya");
    ASSERT(!contacts[2].birthday);
    ASSERT(contacts[2].phones.empty());
}

void TestFindNameByPrefix() {
    PhoneBook book({
                           {"Vasiliy Petrov", std::nullopt, {}},
                           {"Ivan Ivanov", std::nullopt, {}},
                           {"Vasiliy Ivanov", std::nullopt, {}},
                           {"Vasilisa Kuznetsova", std::nullopt, {}},
                           {"Ivan Petrov", std::nullopt, {}},
                           {"Vassisualiy Lokhankin", std::nullopt, {}},
                           {"Ivan Vasiliev", std::nullopt, {}},
                           {"", std::nullopt, {}},
                   });

    auto get_names = [](PhoneBook::ContactRange range) {
        vector<string> result;
        for (const auto& record : range) {
            result.push_back(record.name);
        }
        return result;
    };
    ASSERT_EQUAL(
            get_names(book.FindByNamePrefix("Ivan")),
            (vector<string>{"Ivan Ivanov", "Ivan Petrov", "Ivan Vasiliev"})
    );
    ASSERT_EQUAL(
            get_names(book.FindByNamePrefix("I")),
            (vector<string>{"Ivan Ivanov", "Ivan Petrov", "Ivan Vasiliev"})
    );
    ASSERT_EQUAL(
            get_names(book.FindByNamePrefix("Iv")),
            (vector<string>{"Ivan Ivanov", "Ivan Petrov", "Ivan Vasiliev"})
    );
    ASSERT_EQUAL(
            get_names(book.FindByNamePrefix("Ivan Ivan")), (vector<string>{"Ivan Ivanov"})
    );
    ASSERT_EQUAL(
            get_names(book.FindByNamePrefix("Vas")),
            (vector<string>{
                    "Vasilisa Kuznetsova",
                    "Vasiliy Ivanov",
                    "Vasiliy Petrov",
                    "Vassisualiy Lokhankin"
            })
    );
    ASSERT_EQUAL(
            get_names(book.FindByNamePrefix("Vasili")),
            (vector<string>{"Vasilisa Kuznetsova", "Vasiliy Ivanov", "Vasiliy Petrov"})
    );
    ASSERT_EQUAL(
            get_names(book.FindByNamePrefix("Vasiliy")),
            (vector<string>{"Vasiliy Ivanov", "Vasiliy Petrov"})
    );
    ASSERT_EQUAL(get_names(book.FindByNamePrefix("Vasilis")), (vector<string>{"Vasilisa Kuznetsova"}));
    ASSERT_EQUAL(
            get_names(book.FindByNamePrefix("")),
            (vector<string>{
                    "",
                    "Ivan Ivanov",
                    "Ivan Petrov",
                    "Ivan Vasiliev",
                    "Vasilisa Kuznetsova",
                    "Vasiliy Ivanov",
                    "Vasiliy Petrov",
                    "Vassisualiy Lokhankin"
            })
    );
}

void TestFindNameByPrefix2() {
    PhoneBook book({
                           {"a", std::nullopt, {}},
                           {"aaaa", std::nullopt, {}},
                           {"aabc", std::nullopt, {}},
                           {"aabccc", std::nullopt, {}},
                           {"aabcbc", std::nullopt, {}},
                           {"aeca", std::nullopt, {}},
                           {"aeca", std::nullopt, {}},
                           {"aefg", std::nullopt, {}},
                           {"aq", std::nullopt, {}},
                   });

    auto get_names = [](PhoneBook::ContactRange range) {
        vector<string> result;
        for (const auto& record : range) {
            result.push_back(record.name);
        }
        return result;
    };

    ASSERT_EQUAL(book.FindByNamePrefix("a").size(), 9u);
    ASSERT_EQUAL(
            get_names(book.FindByNamePrefix("aa")),
            (vector<string>{"aaaa", "aabc", "aabcbc", "aabccc"})
    );

    ASSERT_EQUAL(book.FindByNamePrefix("ab").size(), 0u);
    ASSERT_EQUAL(book.FindByNamePrefix("b").size(), 0u);
    ASSERT_EQUAL(book.FindByNamePrefix("aaaaa").size(), 0u);
    ASSERT_EQUAL(book.FindByNamePrefix("aeb").size(), 0u);
    ASSERT_EQUAL(book.FindByNamePrefix("aed").size(), 0u);
    ASSERT_EQUAL(book.FindByNamePrefix("aeg").size(), 0u);

    ASSERT_EQUAL(get_names(book.FindByNamePrefix("aaa")), (vector<string>{"aaaa"}));
    ASSERT_EQUAL(
            get_names(book.FindByNamePrefix("ae")),
            (vector<string>{"aeca", "aeca", "aefg"})
    );
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestFindNameByPrefix);
    RUN_TEST(tr, TestFindNameByPrefix2);
    RUN_TEST(tr, TestSerialization);
    RUN_TEST(tr, TestDeserialization);
}
