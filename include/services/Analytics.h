#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <utility>
using namespace std;

class Analytics {
unordered_map<string, int> hashtag_counts;
public:
void ingestHashtags(const vector<string> &tags);
vector<std::pair<string,int>> topHashtags(int k=10) const;
const unordered_map<string, int>& getHashtagCounts() const { return hashtag_counts; }
void setHashtagCounts(const unordered_map<string, int>& counts) { hashtag_counts = counts; }
};