#pragma once
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include "../models/User.h"
using namespace std;
class GraphService {
unordered_map<UserID, vector<UserID>> adj;
public:
void addUser(UserID uid);
void addFriendship(UserID a, UserID b);
const vector<UserID>& neighbors(UserID uid) const;
vector<vector<UserID>> connected_components() const;
vector<UserID> suggestFriends(UserID uid, int k=10) const;
};