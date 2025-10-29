#pragma once
#include <string>
#include <chrono>

using PostID = uint64_t;
using UserID = uint64_t;
using namespace std;

struct Message {
uint64_t id{0};
UserID from{0};
UserID to{0};
string text;
chrono::system_clock::time_point sent_at;


Message() = default;
Message(uint64_t id_, UserID f, UserID t, const string &m)
: id(id_), from(f), to(t), text(m), sent_at(chrono::system_clock::now()) {}
};