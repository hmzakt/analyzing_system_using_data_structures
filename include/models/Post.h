#include <string>
#include <vector>
#include <chrono>
#include <cstdint>


using PostID = uint64_t;
using UserID = uint64_t;
using namespace std;

struct Post {
PostID id{0};
UserID author{0};
string text;
vector<std::string> hashtags;
uint64_t likes{0};
chrono::system_clock::time_point timestamp;


Post() = default;
Post(PostID id_, UserID auth, const std::string &t)
: id(id_), author(auth), text(t), timestamp(chrono::system_clock::now()) {}
};
