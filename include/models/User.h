#pragma once
#include <string>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <cstdint>

using namespace std;
using UserID = uint64_t;

struct User {
UserID id{0};
string username;
string display_name;
string bio;
unordered_set<UserID> friends;
vector<UserID> followers; 
uint64_t posts_count{0};
uint64_t likes_received{0};
chrono::system_clock::time_point created_at;


User() = default;
User(UserID id_, const string &uname, const string &dname="")
: id(id_), username(uname), display_name(dname), created_at(chrono::system_clock::now()) {}
};