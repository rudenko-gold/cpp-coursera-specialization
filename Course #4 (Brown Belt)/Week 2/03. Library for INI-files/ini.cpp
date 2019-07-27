#include "ini.h"

namespace Ini {
    Document Load(istream &input) {
        Document result;
        string section_name;
        string current_line;

        while(getline(input, current_line)) {
            if (current_line == "") {
                continue;
            }

            if (current_line[0] == '[') {
                section_name = string(current_line.begin() + 1, current_line.end() - 1);
                result.AddSection(section_name);
            } else {
                auto equal_char_it = find(current_line.begin(), current_line.end(), '=');

                string key(current_line.begin(), equal_char_it);
                string value(equal_char_it + 1, current_line.end());

                result.AddSection(section_name).insert({key, value});
            }
        }

        return result;
    }

    Section& Document::AddSection(string name) {
        return sections[name];
    }

    const Section& Document::GetSection(const string &name) const {
        return sections.at(name);
    }

    size_t Document::SectionCount() const {
        return sections.size();
    }
}
