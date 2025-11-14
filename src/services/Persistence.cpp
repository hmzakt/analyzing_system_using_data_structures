#include "../../include/services/Persistence.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Simple JSON-like persistence (without external dependencies)
// Format: simple text-based format

bool Persistence::saveUsers(const vector<User> &users) const {
    string filepath = data_dir + "/users.json";
    ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "[\n";
    for (size_t i = 0; i < users.size(); i++) {
        const User& u = users[i];
        file << "  {\n";
        file << "    \"id\": " << u.id << ",\n";
        file << "    \"username\": \"" << u.username << "\",\n";
        file << "    \"display_name\": \"" << u.display_name << "\",\n";
        file << "    \"posts_count\": " << u.posts_count << "\n";
        file << "  }";
        if (i < users.size() - 1) file << ",";
        file << "\n";
    }
    file << "]\n";
    file.close();
    return true;
}

bool Persistence::loadUsers(vector<User> &out) const {
    string filepath = data_dir + "/users.json";
    ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple parser - just skip for now, return empty
    // In a real implementation, you'd parse JSON properly
    file.close();
    return true;
}

bool Persistence::savePosts(const vector<Post> &posts) const {
    string filepath = data_dir + "/posts.json";
    ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "[\n";
    for (size_t i = 0; i < posts.size(); i++) {
        const Post& p = posts[i];
        file << "  {\n";
        file << "    \"id\": " << p.id << ",\n";
        file << "    \"author\": " << p.author << ",\n";
        file << "    \"text\": \"" << p.text << "\",\n";
        file << "    \"likes\": " << p.likes << "\n";
        file << "  }";
        if (i < posts.size() - 1) file << ",";
        file << "\n";
    }
    file << "]\n";
    file.close();
    return true;
}

bool Persistence::loadPosts(vector<Post> &out) const {
    string filepath = data_dir + "/posts.json";
    ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple parser - just skip for now, return empty
    // In a real implementation, you'd parse JSON properly
    file.close();
    return true;
}

