@echo off
echo Building Social Network Project...

g++ -std=c++17 -I include src/main.cpp src/ds/Trie.cpp src/ds/SegmentTree.cpp src/models/Post.cpp src/models/User.cpp src/services/GraphService.cpp src/services/SearchService.cpp src/services/MessagingService.cpp src/services/Analytics.cpp src/services/RankingService.cpp src/services/Persistence.cpp -o socialnet.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful! Run with: socialnet.exe
) else (
    echo Build failed!
    exit /b 1
)

