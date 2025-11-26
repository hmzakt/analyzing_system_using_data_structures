#pragma once
#include <queue>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include "../models/Message.h"

using namespace std;
class MessagingService {
queue<Message> outbox;
unordered_map<UserID, vector<Message>> inbox;
uint64_t next_msg_id{1};
public:
Message createMessage(UserID from, UserID to, const std::string &text);
void sendMessage(const Message &m);
vector<Message> receive(UserID uid);
const unordered_map<UserID, vector<Message>>& getInbox() const { return inbox; }
uint64_t getNextMsgId() const { return next_msg_id; }
void setInbox(const unordered_map<UserID, vector<Message>>& newInbox) { inbox = newInbox; }
void setNextMsgId(uint64_t id) { next_msg_id = id; }
};