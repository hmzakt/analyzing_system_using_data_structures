#pragma once
#include <vector>
#include <algorithm>
#include "../models/User.h"
using namespace std;

class RankingService {
public:
//Score = followers + likes
static vector<UserID> rankByPopularity(const vector<User> &users, int topk=50);
};