#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>
#include "../ds/Trie.h"
using namespace std;
using UserID = uint64_t;
class SearchService {
Trie trie;
unordered_map<string, UserID> name_to_id;
public:
void indexUser(const std::string &username, UserID id);
vector<string> autocomplete(const string &prefix, int limit=10) const;
bool lookup(const string &username, UserID &out) const;
};