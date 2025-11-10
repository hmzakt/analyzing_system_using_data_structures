#include <queue>
#include <unordered_map>
#include <vector>
#include "../models/Message.h"


class MessagingService {
queue<Message> outbox;
unordered_map<UserID, vector<Message>> inbox;
uint64_t next_msg_id{1};
public:
Message createMessage(UserID from, UserID to, const std::string &text);
void sendMessage(const Message &m);
vector<Message> receive(UserID uid);
};