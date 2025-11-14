#include "../../include/services/MessagingService.h"

Message MessagingService::createMessage(UserID from, UserID to, const std::string &text) {
    Message msg(next_msg_id++, from, to, text);
    return msg;
}

void MessagingService::sendMessage(const Message &m) {
    outbox.push(m);
    inbox[m.to].push_back(m);
}

vector<Message> MessagingService::receive(UserID uid) {
    auto it = inbox.find(uid);
    if (it != inbox.end()) {
        return it->second;
    }
    return vector<Message>();
}

