#include "../../include/services/SearchService.h"

void SearchService::indexUser(const std::string &username, UserID id) {
    name_to_id[username] = id;
    trie.insert(username);
}

vector<string> SearchService::autocomplete(const string &prefix, int limit) const {
    return trie.autocomplete(prefix, limit);
}

bool SearchService::lookup(const string &username, UserID &out) const {
    auto it = name_to_id.find(username);
    if (it != name_to_id.end()) {
        out = it->second;
        return true;
    }
    return false;
}

