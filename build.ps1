# Build script for Social Network Project
Write-Host "Building Social Network Project..." -ForegroundColor Green

$sources = @(
    "src/main.cpp",
    "src/ds/Trie.cpp",
    "src/ds/SegmentTree.cpp",
    "src/models/Post.cpp",
    "src/models/User.cpp",
    "src/services/GraphService.cpp",
    "src/services/SearchService.cpp",
    "src/services/MessagingService.cpp",
    "src/services/Analytics.cpp",
    "src/services/RankingService.cpp",
    "src/services/Persistence.cpp"
)

$includes = "-I include"
$flags = "-std=c++17"
$output = "socialnet.exe"

$command = "g++ $flags $includes $($sources -join ' ') -o $output"

Write-Host "Running: $command" -ForegroundColor Yellow
Invoke-Expression $command

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful! Run with: .\socialnet.exe" -ForegroundColor Green
} else {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

