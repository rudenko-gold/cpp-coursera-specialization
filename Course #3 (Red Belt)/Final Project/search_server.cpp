#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <utility>
#include <future>
#include <mutex>
#include <array>
#include <future>
#include <functional>

vector<string> SplitIntoWords(const string& line) {
    istringstream words_input(line);
    return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

void InvertedIndex::Add(const string& document) {
    docs.push_back(document);
    const size_t docid = docs.size() - 1;

    for (const auto& word : SplitIntoWords(document)) {
        vector<pair<size_t, size_t>>& wordRecord = index[word];
        if (wordRecord.empty()) {
            wordRecord.push_back({docid, 1});
        } else if (wordRecord.back().first != docid) {
            wordRecord.push_back({docid, 1});
        } else {
            wordRecord.back().second++;
        }
    }
}

const vector<pair<size_t, size_t>>& InvertedIndex::Lookup(const string& word) {
    return index[word];
}

SearchServer::SearchServer(istream& document_input) {
    UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
    future<void> f = async([&document_input, this]() {
        InvertedIndex new_index;
        string current_document;
        while (getline(document_input, current_document)) {
            new_index.Add(move(current_document));
        }

        // Get Access to index reference
        index.GetAccess().ref_to_value = move(new_index);
        // Unlock index
    });
}

void SearchServer::AddQueriesStream(
        istream& query_input, ostream& search_results_output
) {
    future<void> f = async([&query_input, this, &search_results_output]() {
        for (string current_query; getline(query_input, current_query); ) {
            InvertedIndex& index_ref = index.GetAccess().ref_to_value;
            const auto words = SplitIntoWords(current_query);

            vector<pair<size_t, size_t>> docid_count(index.GetAccess().ref_to_value.size(), {0, 0});

            for (size_t i = 0; i < docid_count.size(); ++i) {
                docid_count[i].first = i;
                docid_count[i].second = 0;
            }

            for (const auto& word : words) {
                for (const pair<size_t, size_t>& docid : index_ref.Lookup(word)) {
                    docid_count[docid.first].second += docid.second;
                }
            }

            partial_sort(
                    begin(docid_count),
                    Head(docid_count, 5).end(),
                    end(docid_count),
                    [](pair<size_t, size_t>& lhs, pair<size_t, size_t>& rhs) {
                        int64_t lhs_docid = lhs.first;
                        auto lhs_hit_count = lhs.second;
                        int64_t rhs_docid = rhs.first;
                        auto rhs_hit_count = rhs.second;
                        return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
                    }
            );

            search_results_output << current_query << ':';
            for (auto [docid, hitcount] : Head(docid_count, 5)) {
                if (hitcount == 0) {
                    break;
                }

                search_results_output << " {"
                                      << "docid: " << docid << ", "
                                      << "hitcount: " << hitcount << '}';
            }
            search_results_output << endl;
        }
    });
}
