#include "../../include/services/GraphService.h"
#include <algorithm>
#include <queue>
#include <unordered_set>

void GraphService::addUser(UserID uid) {
    if (adj.find(uid) == adj.end()) {
        adj[uid] = vector<UserID>();
    }
}

void GraphService::addFriendship(UserID a, UserID b) {
    addUser(a);
    addUser(b);
    // Add bidirectional edge
    if (find(adj[a].begin(), adj[a].end(), b) == adj[a].end()) {
        adj[a].push_back(b);
    }
    if (find(adj[b].begin(), adj[b].end(), a) == adj[b].end()) {
        adj[b].push_back(a);
    }
}

const vector<UserID>& GraphService::neighbors(UserID uid) const {
    static const vector<UserID> empty;
    auto it = adj.find(uid);
    if (it != adj.end()) {
        return it->second;
    }
    return empty;
}

vector<vector<UserID>> GraphService::connected_components() const {
    vector<vector<UserID>> components;
    unordered_set<UserID> visited;
    
    for (const auto& pair : adj) {
        UserID start = pair.first;
        if (visited.find(start) != visited.end()) continue;
        
        vector<UserID> component;
        queue<UserID> q;
        q.push(start);
        visited.insert(start);
        
        while (!q.empty()) {
            UserID u = q.front();
            q.pop();
            component.push_back(u);
            
            auto it = adj.find(u);
            if (it != adj.end()) {
                for (UserID v : it->second) {
                    if (visited.find(v) == visited.end()) {
                        visited.insert(v);
                        q.push(v);
                    }
                }
            }
        }
        components.push_back(component);
    }
    
    return components;
}

vector<UserID> GraphService::suggestFriends(UserID uid, int k) const {
    unordered_set<UserID> friends;
    auto it = adj.find(uid);
    if (it != adj.end()) {
        for (UserID f : it->second) {
            friends.insert(f);
        }
    }
    friends.insert(uid); // exclude self
    
    // Count mutual friends
    unordered_map<UserID, int> mutual_count;
    for (UserID friend_id : friends) {
        if (friend_id == uid) continue;
        auto friend_it = adj.find(friend_id);
        if (friend_it != adj.end()) {
            for (UserID mutual : friend_it->second) {
                if (friends.find(mutual) == friends.end() && mutual != uid) {
                    mutual_count[mutual]++;
                }
            }
        }
    }
    
    // Sort by mutual friend count
    vector<pair<int, UserID>> candidates;
    for (const auto& p : mutual_count) {
        candidates.push_back({p.second, p.first});
    }
    sort(candidates.begin(), candidates.end(), [](const pair<int, UserID>& a, const pair<int, UserID>& b) {
        return a.first > b.first;
    });
    
    vector<UserID> result;
    for (int i = 0; i < min(k, (int)candidates.size()); i++) {
        result.push_back(candidates[i].second);
    }
    
    return result;
}

