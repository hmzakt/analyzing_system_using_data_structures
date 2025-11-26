#include "../../include/services/Persistence.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

// Helper to escape JSON strings
string escapeJson(const string& s) {
    string escaped;
    for (char c : s) {
        if (c == '"') escaped += "\\\"";
        else if (c == '\\') escaped += "\\\\";
        else if (c == '\n') escaped += "\\n";
        else if (c == '\r') escaped += "\\r";
        else if (c == '\t') escaped += "\\t";
        else escaped += c;
    }
    return escaped;
}

// Helper to unescape JSON strings
string unescapeJson(const string& s) {
    string result;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            if (s[i+1] == '"') { result += '"'; i++; }
            else if (s[i+1] == '\\') { result += '\\'; i++; }
            else if (s[i+1] == 'n') { result += '\n'; i++; }
            else if (s[i+1] == 'r') { result += '\r'; i++; }
            else if (s[i+1] == 't') { result += '\t'; i++; }
            else result += s[i];
        } else {
            result += s[i];
        }
    }
    return result;
}

// Simple JSON parser helpers
class JSONParser {
    string content;
    size_t pos;
    
    void skipWhitespace() {
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || 
               content[pos] == '\t' || content[pos] == '\r')) {
            pos++;
        }
    }
    
public:
    JSONParser(const string& json) : content(json), pos(0) {}
    
    uint64_t parseNumber() {
        skipWhitespace();
        size_t start = pos;
        while (pos < content.size() && isdigit(content[pos])) pos++;
        return stoull(content.substr(start, pos - start));
    }
    
    string parseString() {
        skipWhitespace();
        if (content[pos] != '"') return "";
        pos++;
        size_t start = pos;
        while (pos < content.size() && content[pos] != '"') {
            if (content[pos] == '\\') pos++;
            pos++;
        }
        string result = content.substr(start, pos - start);
        pos++;
        return unescapeJson(result);
    }
    
    vector<uint64_t> parseNumberArray() {
        vector<uint64_t> result;
        skipWhitespace();
        if (content[pos] != '[') return result;
        pos++;
        
        while (pos < content.size()) {
            skipWhitespace();
            if (content[pos] == ']') {
                pos++;
                break;
            }
            result.push_back(parseNumber());
            skipWhitespace();
            if (content[pos] == ',') pos++;
        }
        return result;
    }
    
    vector<string> parseStringArray() {
        vector<string> result;
        skipWhitespace();
        if (content[pos] != '[') return result;
        pos++;
        
        while (pos < content.size()) {
            skipWhitespace();
            if (content[pos] == ']') {
                pos++;
                break;
            }
            result.push_back(parseString());
            skipWhitespace();
            if (content[pos] == ',') pos++;
        }
        return result;
    }
    
    bool expect(char c) {
        skipWhitespace();
        if (pos < content.size() && content[pos] == c) {
            pos++;
            return true;
        }
        return false;
    }
    
    string parseKey() {
        return parseString();
    }
    
    void skipValue() {
        skipWhitespace();
        if (content[pos] == '"') {
            parseString();
        } else if (content[pos] == '[' || content[pos] == '{') {
            int depth = 1;
            char open = content[pos++];
            char close = (open == '[') ? ']' : '}';
            while (pos < content.size() && depth > 0) {
                if (content[pos] == open) depth++;
                else if (content[pos] == close) depth--;
                pos++;
            }
        } else {
            while (pos < content.size() && content[pos] != ',' && content[pos] != '}' && content[pos] != ']') {
                pos++;
            }
        }
    }
};

// Save/Load Users
bool Persistence::saveUsers(const unordered_map<UserID, User> &users) const {
    ofstream file(data_dir + "/users.json");
    if (!file.is_open()) return false;
    
    file << "[\n";
    size_t count = 0;
    for (const auto& pair : users) {
        const User& u = pair.second;
        file << "  {\n";
        file << "    \"id\": " << u.id << ",\n";
        file << "    \"username\": \"" << escapeJson(u.username) << "\",\n";
        file << "    \"display_name\": \"" << escapeJson(u.display_name) << "\",\n";
        file << "    \"posts_count\": " << u.posts_count << ",\n";
        
        file << "    \"friends\": [";
        size_t fi = 0;
        for (UserID fid : u.friends) {
            file << fid;
            if (++fi < u.friends.size()) file << ",";
        }
        file << "],\n";
        
        file << "    \"followers\": [";
        for (size_t i = 0; i < u.followers.size(); ++i) {
            file << u.followers[i];
            if (i < u.followers.size() - 1) file << ",";
        }
        file << "]\n";
        
        file << "  }";
        if (++count < users.size()) file << ",";
        file << "\n";
    }
    file << "]\n";
    file.close();
    return true;
}

bool Persistence::loadUsers(unordered_map<UserID, User> &out, UserID &nextUserId) const {
    ifstream file(data_dir + "/users.json");
    if (!file.is_open()) return false;
    
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    
    if (content.empty() || content == "[]\n" || content == "[]") return true;
    
    JSONParser parser(content);
    if (!parser.expect('[')) return false;
    
    while (true) {
        if (parser.expect(']')) break;
        if (!parser.expect('{')) return false;
        
        User u;
        while (true) {
            if (parser.expect('}')) break;
            
            string key = parser.parseKey();
            if (!parser.expect(':')) return false;
            
            if (key == "id") {
                u.id = parser.parseNumber();
                if (u.id >= nextUserId) nextUserId = u.id + 1;
            } else if (key == "username") {
                u.username = parser.parseString();
            } else if (key == "display_name") {
                u.display_name = parser.parseString();
            } else if (key == "posts_count") {
                u.posts_count = parser.parseNumber();
            } else if (key == "friends") {
                auto friendIds = parser.parseNumberArray();
                for (auto id : friendIds) u.friends.insert(id);
            } else if (key == "followers") {
                auto followerIds = parser.parseNumberArray();
                u.followers = followerIds;
            } else {
                parser.skipValue();
            }
            
            parser.expect(',');
        }
        
        out[u.id] = u;
        parser.expect(',');
    }
    
    return true;
}

// Save/Load Posts
bool Persistence::savePosts(const unordered_map<PostID, Post> &posts) const {
    ofstream file(data_dir + "/posts.json");
    if (!file.is_open()) return false;
    
    file << "[\n";
    size_t count = 0;
    for (const auto& pair : posts) {
        const Post& p = pair.second;
        file << "  {\n";
        file << "    \"id\": " << p.id << ",\n";
        file << "    \"author\": " << p.author << ",\n";
        file << "    \"text\": \"" << escapeJson(p.text) << "\",\n";
        file << "    \"likes\": " << p.likes << ",\n";
        file << "    \"hashtags\": [";
        for (size_t i = 0; i < p.hashtags.size(); ++i) {
            file << "\"" << escapeJson(p.hashtags[i]) << "\"";
            if (i < p.hashtags.size() - 1) file << ",";
        }
        file << "]\n";
        file << "  }";
        if (++count < posts.size()) file << ",";
        file << "\n";
    }
    file << "]\n";
    file.close();
    return true;
}

bool Persistence::loadPosts(unordered_map<PostID, Post> &out, PostID &nextPostId) const {
    ifstream file(data_dir + "/posts.json");
    if (!file.is_open()) return false;
    
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    
    if (content.empty() || content == "[]\n" || content == "[]") return true;
    
    JSONParser parser(content);
    if (!parser.expect('[')) return false;
    
    while (true) {
        if (parser.expect(']')) break;
        if (!parser.expect('{')) return false;
        
        Post p;
        while (true) {
            if (parser.expect('}')) break;
            
            string key = parser.parseKey();
            if (!parser.expect(':')) return false;
            
            if (key == "id") {
                p.id = parser.parseNumber();
                if (p.id >= nextPostId) nextPostId = p.id + 1;
            } else if (key == "author") {
                p.author = parser.parseNumber();
            } else if (key == "text") {
                p.text = parser.parseString();
            } else if (key == "likes") {
                p.likes = parser.parseNumber();
            } else if (key == "hashtags") {
                p.hashtags = parser.parseStringArray();
            } else {
                parser.skipValue();
            }
            
            parser.expect(',');
        }
        
        out[p.id] = p;
        parser.expect(',');
    }
    
    return true;
}

// Save/Load Messages
bool Persistence::saveMessages(const unordered_map<UserID, vector<Message>> &inbox, uint64_t nextMsgId) const {
    ofstream file(data_dir + "/messages.json");
    if (!file.is_open()) return false;
    
    file << "{\n";
    file << "  \"next_msg_id\": " << nextMsgId << ",\n";
    file << "  \"inbox\": {\n";
    
    size_t userCount = 0;
    for (const auto& pair : inbox) {
        UserID userId = pair.first;
        const vector<Message>& messages = pair.second;
        file << "    \"" << userId << "\": [\n";
        for (size_t i = 0; i < messages.size(); ++i) {
            const Message& m = messages[i];
            file << "      {\n";
            file << "        \"id\": " << m.id << ",\n";
            file << "        \"from\": " << m.from << ",\n";
            file << "        \"to\": " << m.to << ",\n";
            file << "        \"text\": \"" << escapeJson(m.text) << "\"\n";
            file << "      }";
            if (i < messages.size() - 1) file << ",";
            file << "\n";
        }
        file << "    ]";
        if (++userCount < inbox.size()) file << ",";
        file << "\n";
    }
    
    file << "  }\n";
    file << "}\n";
    file.close();
    return true;
}

bool Persistence::loadMessages(unordered_map<UserID, vector<Message>> &inbox, uint64_t &nextMsgId) const {
    ifstream file(data_dir + "/messages.json");
    if (!file.is_open()) return false;
    
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    
    if (content.empty() || content == "{}\n" || content == "{}") return true;
    
    JSONParser parser(content);
    if (!parser.expect('{')) return false;
    
    while (true) {
        if (parser.expect('}')) break;
        
        string key = parser.parseKey();
        if (!parser.expect(':')) return false;
        
        if (key == "next_msg_id") {
            nextMsgId = parser.parseNumber();
        } else if (key == "inbox") {
            if (!parser.expect('{')) return false;
            
            while (true) {
                if (parser.expect('}')) break;
                
                UserID userId = stoull(parser.parseString());
                if (!parser.expect(':')) return false;
                if (!parser.expect('[')) return false;
                
                vector<Message> messages;
                while (true) {
                    if (parser.expect(']')) break;
                    if (!parser.expect('{')) return false;
                    
                    Message m;
                    while (true) {
                        if (parser.expect('}')) break;
                        
                        string msgKey = parser.parseKey();
                        if (!parser.expect(':')) return false;
                        
                        if (msgKey == "id") m.id = parser.parseNumber();
                        else if (msgKey == "from") m.from = parser.parseNumber();
                        else if (msgKey == "to") m.to = parser.parseNumber();
                        else if (msgKey == "text") m.text = parser.parseString();
                        else parser.skipValue();
                        
                        parser.expect(',');
                    }
                    messages.push_back(m);
                    parser.expect(',');
                }
                
                inbox[userId] = messages;
                parser.expect(',');
            }
        } else {
            parser.skipValue();
        }
        
        parser.expect(',');
    }
    
    return true;
}

// Save/Load Analytics
bool Persistence::saveAnalytics(const unordered_map<string, int> &hashtagCounts) const {
    ofstream file(data_dir + "/analytics.json");
    if (!file.is_open()) return false;
    
    file << "{\n";
    size_t count = 0;
    for (const auto& pair : hashtagCounts) {
        file << "  \"" << escapeJson(pair.first) << "\": " << pair.second;
        if (++count < hashtagCounts.size()) file << ",";
        file << "\n";
    }
    file << "}\n";
    file.close();
    return true;
}

bool Persistence::loadAnalytics(unordered_map<string, int> &hashtagCounts) const {
    ifstream file(data_dir + "/analytics.json");
    if (!file.is_open()) return false;
    
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    
    if (content.empty() || content == "{}\n" || content == "{}") return true;
    
    JSONParser parser(content);
    if (!parser.expect('{')) return false;
    
    while (true) {
        if (parser.expect('}')) break;
        
        string tag = parser.parseKey();
        if (!parser.expect(':')) return false;
        int count = parser.parseNumber();
        
        hashtagCounts[tag] = count;
        parser.expect(',');
    }
    
    return true;
}
