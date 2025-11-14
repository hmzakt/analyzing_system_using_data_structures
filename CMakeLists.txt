cmake_minimum_required(VERSION 3.10)
project(SocialNetwork)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/ds/Trie.cpp
    src/ds/SegmentTree.cpp
    src/models/Post.cpp
    src/models/User.cpp
    src/services/GraphService.cpp
    src/services/SearchService.cpp
    src/services/MessagingService.cpp
    src/services/Analytics.cpp
    src/services/RankingService.cpp
    src/services/Persistence.cpp
)

# Create executable
add_executable(socialnet ${SOURCES})

# Set output directory
set_target_properties(socialnet PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

