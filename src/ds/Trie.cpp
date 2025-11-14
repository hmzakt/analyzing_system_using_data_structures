#include "../../include/ds/Trie.h"
#include <queue>
#include <algorithm>
using namespace std;


Trie::Trie() : root(make_unique<Node>()) {}


void Trie::insert(const string &s, int freq) {
Node *cur = root.get();
for(char c : s) {
if(!cur->children.count(c)) cur->children[c] = make_unique<Node>();
cur = cur->children[c].get();
}
cur->end_of_word = true;
cur->freq += freq;
}


bool Trie::contains(const string &s) const {
Node *cur = root.get();
for(char c : s) {
auto it = cur->children.find(c);
if(it == cur->children.end()) return false;
cur = it->second.get();
}
return cur->end_of_word;
}


vector<string> Trie::autocomplete(const string &prefix, int limit) const {
vector<string> res;
const Node *cur = root.get();
for(char c : prefix) {
auto it = cur->children.find(c);
if(it == cur->children.end()) return res;
cur = it->second.get();
}

using Item = pair<int,string>;
vector<Item> found;
queue<pair<const Node*, string>> q;
q.push({cur, prefix});
while(!q.empty()) {
auto p = q.front(); q.pop();
const Node* node = p.first;
const string &s = p.second;
if(node->end_of_word) found.push_back({node->freq, s});
for(auto &ch : node->children) {
q.push({ch.second.get(), s + ch.first});
}
}
sort(found.begin(), found.end(), [](const Item &a, const Item &b){
return a.first > b.first; // higher freq first
});
for(int i=0;i<min<int>((int)found.size(), limit);++i) res.push_back(found[i].second);
return res;
}