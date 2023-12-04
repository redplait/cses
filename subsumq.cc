#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <map>
#include <vector>

// solution for Subarray Sum Queries
// almost the same as in psq.cc
using namespace std;

struct item
{
  int64_t prefix, sum, suffix, res;
  item(): prefix(0), sum(0), suffix(0), res(0)
  { }
  item(int64_t n): prefix(n), sum(n), suffix(n), res(n)
  { }
  item(int64_t _p, int64_t suff, int64_t _sum, int64_t _r): prefix(_p), sum(_sum), suffix(suff), res(_r)
  { }
};

// probably better to make it as template class but I am too lazy
struct Segment_Tree
{
    vector<item> segtree;
    Segment_Tree(int n): segtree(4 * n)
    { }
    Segment_Tree(vector<int> &a)
    {
      int n = (int)a.size();
      segtree = vector<item>(4 * n);
      build(a, 0, n - 1, 1);
    }
    item merge(item &a, item &b)
    {
      int64_t prefix = max(a.prefix, a.sum + b.prefix);
      int64_t suffix = max(b.suffix, a.suffix + b.sum);
      int64_t sum = a.sum + b.sum;
      int64_t res = max(a.suffix + b.prefix, max(a.res, b.res));
      return item(prefix, suffix, sum, res);
    }
    void build(vector<int> &a, int s, int e, int root)
    {
      if (s == e) {
	segtree[root] = item(a[s]);
	return;
      }
      int mid = (s + e) / 2;
      build(a, s, mid, 2 * root);
      build(a, mid + 1, e, 2 * root + 1);

      segtree[root] = merge(segtree[2 * root], segtree[2 * root + 1]);
    }
    void update(int s, int e, int root, int idx, int val)
    {
       if (s == e) {
	segtree[root] = item(val);
	return;
       }
       int mid = (s + e) / 2;
       if (mid >= idx) update(s, mid, 2 * root, idx, val);
       else update(mid + 1, e, 2 * root + 1, idx, val);
       segtree[root] = merge(segtree[2 * root], segtree[2 * root + 1]);
    }
    item query(int s, int e, int root, int l, int r) {  //query with root index set to 1
	// completely out of range
	if (s > r || e < l)
	    return item(LONG_MIN);
	// completely in range
	if (s >= l && e <= r) {
	    return segtree[root];
	}
	// partial overlap
	int mid = (s + e) / 2;
	auto x = query(s, mid, 2 * root, l, r);
	auto y = query(mid + 1, e, 2 * root + 1, l, r);
	return merge(x, y);
    }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n,q;
  cin>>n>>q;
  vector<int> a(n);
  for (int i = 0; i < n; i++) cin>>a[i];
  Segment_Tree st(a);
  while(q--)
  {
     int idx, val;
     cin>>idx>>val;
     idx--;
     st.update(0, n - 1, 1, idx, val);
     printf("%ld\n", max((int64_t)0, st.segtree[1].res));
  }
}