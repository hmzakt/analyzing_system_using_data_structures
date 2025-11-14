#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "../models/User.h"
#include "../models/Post.h"
using namespace std;

// Simple JSON persistence using nlohmann::json (put json.hpp in extern/)
class Persistence {
string data_dir{"data"};
public:
Persistence() = default;
Persistence(const string &dir) : data_dir(dir) {}
bool saveUsers(const vector<User> &users) const;
bool loadUsers(vector<User> &out) const;
bool savePosts(const vector<Post> &posts) const;
bool loadPosts(vector<Post> &out) const;
};