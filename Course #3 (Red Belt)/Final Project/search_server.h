#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <future>

#include "synchronized.h"

using namespace std;

class InvertedIndex {
public:
    void Add(const string& document);
    const vector<pair<size_t, size_t>>& Lookup(const string& word);
    const string& GetDocument(size_t id) const {
        return docs[id];
    }

    size_t size() {
        return docs.size();
    }
    
private:
    unordered_map<string, vector<pair<size_t, size_t>>> index;
    vector<string> docs;
};

class SearchServer {
public:
    SearchServer() = default;
    explicit SearchServer(istream& document_input);
    void UpdateDocumentBase(istream& document_input);
    void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
    Synchronized<InvertedIndex> index;
};
