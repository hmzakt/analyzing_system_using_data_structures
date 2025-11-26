#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "../models/User.h"
#include "../models/Post.h"
#include "../models/Message.h"
using namespace std;

// Simple JSON persistence using nlohmann::json (put json.hpp in extern/)
class Persistence {
string data_dir{"data"};
public:
Persistence() = default;
Persistence(const string &dir) : data_dir(dir) {}
bool saveUsers(const unordered_map<UserID, User> &users) const;
bool loadUsers(unordered_map<UserID, User> &out, UserID &nextUserId) const;
bool savePosts(const unordered_map<PostID, Post> &posts) const;
bool loadPosts(unordered_map<PostID, Post> &out, PostID &nextPostId) const;
bool saveMessages(const unordered_map<UserID, vector<Message>> &inbox, uint64_t nextMsgId) const;
bool loadMessages(unordered_map<UserID, vector<Message>> &inbox, uint64_t &nextMsgId) const;
bool saveAnalytics(const unordered_map<string, int> &hashtagCounts) const;
bool loadAnalytics(unordered_map<string, int> &hashtagCounts) const;
};