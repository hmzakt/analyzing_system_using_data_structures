#include "../../include/services/RankingService.h"
#include <algorithm>
#include <utility>

vector<UserID> RankingService::rankByPopularity(const vector<User> &users, int topk) {
    vector<pair<int, UserID>> scores;
    
    for (const auto& user : users) {
        // Score = followers + likes_received
        int score = (int)user.followers.size() + (int)user.likes_received;
        scores.push_back({score, user.id});
    }
    
    // Sort by score (descending)
    sort(scores.begin(), scores.end(), [](const pair<int, UserID>& a, const pair<int, UserID>& b) {
        return a.first > b.first;
    });
    
    vector<UserID> result;
    int limit = min(topk, (int)scores.size());
    for (int i = 0; i < limit; i++) {
        result.push_back(scores[i].second);
    }
    
    return result;
}

