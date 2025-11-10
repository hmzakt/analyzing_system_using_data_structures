#include <vector>
using namespace std;
class SegmentTree {
int n;
vector<int> tree; // 1-indexed
public:
SegmentTree(int size);
void build();
void add(int idx, int delta); 
int rangeQuery(int l, int r); 
};