#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;
// for username/hashtag autocomplete
class Trie {
struct Node {
unordered_map<char, unique_ptr<Node>> children;
bool end_of_word{false};
int freq{0}; 
};

unique_ptr<Node> root;
public:
Trie();
void insert(const string &s, int freq=1);
bool contains(const string &s) const;
vector<string> autocomplete(const string &prefix, int limit=10) const;
};