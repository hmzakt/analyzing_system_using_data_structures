#include "../../include/ds/SegmentTree.h"
#include <stdexcept>
using namespace std;


SegmentTree::SegmentTree(int size) {
n = 1;
while(n < size) n <<= 1;
tree.assign(2*n, 0);
}


void SegmentTree::build() {
}


void SegmentTree::add(int idx, int delta) {
if(idx < 0 || idx >= n) throw out_of_range("SegmentTree index out of range");
int i = idx + n;
tree[i] += delta;
while(i > 1) {
i >>= 1;
tree[i] = tree[i<<1] + tree[(i<<1)|1];
}
}


int SegmentTree::rangeQuery(int l, int r) {
if(l>r) return 0;
int res = 0;
int L = l + n, R = r + n;
while(L <= R) {
if(L & 1) res += tree[L++];
if(!(R & 1)) res += tree[R--];
L >>= 1; R >>= 1;
}
return res;
}