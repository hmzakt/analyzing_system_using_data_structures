// src/main.cpp - Simplified Social Network CLI
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <regex>
#include <sstream>

#include "../include/models/User.h"
#include "../include/models/Post.h"
#include "../include/models/Message.h"
#include "../include/services/GraphService.h"
#include "../include/services/SearchService.h"
#include "../include/services/MessagingService.h"
#include "../include/services/Analytics.h"
#include "../include/services/RankingService.h"

using namespace std;

// Helper: Extract hashtags from text
vector<string> extractHashtags(const string& text) {
    vector<string> tags;
    regex pattern(R"(#(\w+))");
    smatch matches;
    string s = text;
    
    while (regex_search(s, matches, pattern)) {
        tags.push_back(matches[1].str());
        s = matches.suffix().str();
    }
    return tags;
}

// Helper: Parse command line
vector<string> parseCommand(const string& line) {
    vector<string> tokens;
    stringstream ss(line);
    string token;
    
    while (ss >> token) {
        // Remove quotes if present
        if (token.front() == '"' && token.back() == '"') {
            token = token.substr(1, token.length() - 2);
        }
        tokens.push_back(token);
    }
    return tokens;
}

void printMenu() {
    cout << "\n=== Social Network CLI ===\n";
    cout << "1. Add User\n";
    cout << "2. Add Friend\n";
    cout << "3. Create Post\n";
    cout << "4. Send Message\n";
    cout << "5. View Inbox\n";
    cout << "6. Search Users\n";
    cout << "7. Friend Suggestions\n";
    cout << "8. Top Trending Hashtags\n";
    cout << "9. Top Users\n";
    cout << "0. Exit\n";
    cout << "Enter choice: ";
}

int main() {
    cout << "Welcome to Social Network!\n";
    
    // Services
    GraphService graph;
    SearchService search;
    MessagingService messenger;
    Analytics analytics;
    
    // Data storage
    unordered_map<UserID, User> users;
    unordered_map<PostID, Post> posts;
    UserID nextUserId = 1;
    PostID nextPostId = 1;
    
    string input;
    int choice;
    
    while (true) {
        printMenu();
        getline(cin, input);
        
        if (input.empty()) continue;
        
        try {
            choice = stoi(input);
        } catch (...) {
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }
        
        switch (choice) {
            case 1: { // Add User
                cout << "Enter username: ";
                string username;
                getline(cin, username);
                
                cout << "Enter display name: ";
                string displayName;
                getline(cin, displayName);
                
                // Check if username exists
                UserID dummy;
                if (search.lookup(username, dummy)) {
                    cout << "Username already exists!\n";
                    break;
                }
                
                User user(nextUserId++, username, displayName);
                users[user.id] = user;
                graph.addUser(user.id);
                search.indexUser(username, user.id);
                
                cout << "User created: " << username << " (ID: " << user.id << ")\n";
                break;
            }
            
            case 2: { // Add Friend
                cout << "Enter first username: ";
                string user1;
                getline(cin, user1);
                
                cout << "Enter second username: ";
                string user2;
                getline(cin, user2);
                
                UserID id1, id2;
                if (!search.lookup(user1, id1) || !search.lookup(user2, id2)) {
                    cout << "One or both users not found!\n";
                    break;
                }
                
                users[id1].friends.insert(id2);
                users[id2].friends.insert(id1);
                users[id1].followers.push_back(id2);
                users[id2].followers.push_back(id1);
                graph.addFriendship(id1, id2);
                
                cout << user1 << " and " << user2 << " are now friends!\n";
                break;
            }
            
            case 3: { // Create Post
                cout << "Enter username: ";
                string username;
                getline(cin, username);
                
                cout << "Enter post text: ";
                string text;
                getline(cin, text);
                
                UserID uid;
                if (!search.lookup(username, uid)) {
                    cout << "User not found!\n";
                    break;
                }
                
                Post post(nextPostId++, uid, text);
                auto hashtags = extractHashtags(text);
                post.hashtags = hashtags;
                posts[post.id] = post;
                users[uid].posts_count++;
                analytics.ingestHashtags(hashtags);
                
                cout << "Post created (ID: " << post.id << ")\n";
                if (!hashtags.empty()) {
                    cout << "Hashtags: ";
                    for (const auto& tag : hashtags) {
                        cout << "#" << tag << " ";
                    }
                    cout << "\n";
                }
                break;
            }
            
            case 4: { // Send Message
                cout << "Enter sender username: ";
                string from;
                getline(cin, from);
                
                cout << "Enter receiver username: ";
                string to;
                getline(cin, to);
                
                cout << "Enter message: ";
                string text;
                getline(cin, text);
                
                UserID fid, tid;
                if (!search.lookup(from, fid) || !search.lookup(to, tid)) {
                    cout << "User(s) not found!\n";
                    break;
                }
                
                auto msg = messenger.createMessage(fid, tid, text);
                messenger.sendMessage(msg);
                cout << "Message sent! (ID: " << msg.id << ")\n";
                break;
            }
            
            case 5: { // View Inbox
                cout << "Enter username: ";
                string username;
                getline(cin, username);
                
                UserID uid;
                if (!search.lookup(username, uid)) {
                    cout << "User not found!\n";
                    break;
                }
                
                auto messages = messenger.receive(uid);
                cout << "\nInbox for " << username << " (" << messages.size() << " messages):\n";
                
                if (messages.empty()) {
                    cout << "No messages.\n";
                } else {
                    for (const auto& msg : messages) {
                        string sender = "(unknown)";
                        auto it = users.find(msg.from);
                        if (it != users.end()) {
                            sender = it->second.username;
                        }
                        cout << "From: " << sender << " | " << msg.text << "\n";
                    }
                }
                break;
            }
            
            case 6: { // Search Users
                cout << "Enter search prefix: ";
                string prefix;
                getline(cin, prefix);
                
                cout << "Max results (default 10): ";
                string limitStr;
                getline(cin, limitStr);
                int limit = limitStr.empty() ? 10 : stoi(limitStr);
                
                auto results = search.autocomplete(prefix, limit);
                cout << "\nSearch results:\n";
                
                if (results.empty()) {
                    cout << "No users found.\n";
                } else {
                    for (const auto& username : results) {
                        UserID uid;
                        if (search.lookup(username, uid)) {
                            auto it = users.find(uid);
                            if (it != users.end()) {
                                cout << "- " << username;
                                if (!it->second.display_name.empty()) {
                                    cout << " (" << it->second.display_name << ")";
                                }
                                cout << "\n";
                            }
                        }
                    }
                }
                break;
            }
            
            case 7: { // Friend Suggestions
                cout << "Enter username: ";
                string username;
                getline(cin, username);
                
                cout << "Number of suggestions (default 10): ";
                string kStr;
                getline(cin, kStr);
                int k = kStr.empty() ? 10 : stoi(kStr);
                
                UserID uid;
                if (!search.lookup(username, uid)) {
                    cout << "User not found!\n";
                    break;
                }
                
                auto suggestions = graph.suggestFriends(uid, k);
                cout << "\nFriend suggestions for " << username << ":\n";
                
                if (suggestions.empty()) {
                    cout << "No suggestions available.\n";
                } else {
                    for (const auto& sugId : suggestions) {
                        auto it = users.find(sugId);
                        if (it != users.end()) {
                            cout << "- " << it->second.username << "\n";
                        }
                    }
                }
                break;
            }
            
            case 8: { // Top Trending Hashtags
                cout << "Number of hashtags (default 10): ";
                string kStr;
                getline(cin, kStr);
                int k = kStr.empty() ? 10 : stoi(kStr);
                
                auto trending = analytics.topHashtags(k);
                cout << "\nTop Trending Hashtags:\n";
                
                if (trending.empty()) {
                    cout << "No hashtags yet.\n";
                } else {
                    for (const auto& pair : trending) {
                        cout << "#" << pair.first << " - " << pair.second << " posts\n";
                    }
                }
                break;
            }
            
            case 9: { // Top Users
                cout << "Number of users (default 10): ";
                string kStr;
                getline(cin, kStr);
                int k = kStr.empty() ? 10 : stoi(kStr);
                
                vector<User> allUsers;
                for (const auto& pair : users) {
                    allUsers.push_back(pair.second);
                }
                
                auto topUsers = RankingService::rankByPopularity(allUsers, k);
                cout << "\nTop Users by Popularity:\n";
                
                if (topUsers.empty()) {
                    cout << "No users yet.\n";
                } else {
                    for (const auto& uid : topUsers) {
                        auto it = users.find(uid);
                        if (it != users.end()) {
                            const User& u = it->second;
                            cout << u.username << " - Followers: " << u.followers.size() 
                                 << ", Posts: " << u.posts_count << "\n";
                        }
                    }
                }
                break;
            }
            
            case 0: // Exit
                cout << "Goodbye!\n";
                return 0;
            
            default:
                cout << "Invalid choice! Please try again.\n";
                break;
        }
    }
    
    return 0;
}
