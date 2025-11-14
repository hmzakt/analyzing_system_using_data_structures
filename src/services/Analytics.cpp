#include "../../include/services/Analytics.h"
#include <algorithm>
#include <vector>

void Analytics::ingestHashtags(const vector<string> &tags) {
    for (const auto& tag : tags) {
        hashtag_counts[tag]++;
    }
}

vector<std::pair<string, int>> Analytics::topHashtags(int k) const {
    vector<std::pair<string, int>> result;
    for (const auto& p : hashtag_counts) {
        result.push_back(p);
    }
    
    // Sort by count (descending)
    sort(result.begin(), result.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second;
    });
    
    if (result.size() > (size_t)k) {
        result.resize(k);
    }
    
    return result;
}

