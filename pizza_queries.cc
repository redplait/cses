#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <vector>

// solution for Pizzeria Queries
// Segment_Tree is the same as in psq.cc
using namespace std;

// probably better to make it as template class but I am too lazy
struct Segment_Tree
{
    vector<int64_t> segtree;
    Segment_Tree(vector<int64_t> &a)
    {
      int n = (int)a.size();
      segtree = vector<int64_t>(4 * n);
      build(a, 0, n - 1, 1);
    }
    inline int64_t merge(int64_t a, int64_t b)
    {
      return a < b ? a : b;
    }
    void build(vector<int64_t> &a, int s, int e, int root)
    {
      if (s == e) {
	segtree[root] = a[s];
	return;
      }
      int mid = (s + e) / 2;
      build(a, s, mid, 2 * root);
      build(a, mid + 1, e, 2 * root + 1);

      segtree[root] = merge(segtree[2 * root], segtree[2 * root + 1]);
    }
    void update(int s, int e, int root, int idx, int64_t val)
    {
       if (s == e) {
	segtree[root] = val;
	return;
       }
       int mid = (s + e) / 2;
       if (mid >= idx) update(s, mid, 2 * root, idx, val);
       else update(mid + 1, e, 2 * root + 1, idx, val);
       segtree[root] = merge(segtree[2 * root], segtree[2 * root + 1]);
    }
    int64_t query(int s, int e, int root, int l, int r) {  //query with root index set to 1
      // completely out of range
      if (s > r || e < l)
         return LONG_MAX;
      // completely in range
      if (s >= l && e <= r) return segtree[root];
      // partial overlap
      int mid = (s + e) / 2;
      auto x = query(s, mid, 2 * root, l, r);
      auto y = query(mid + 1, e, 2 * root + 1, l, r);
      return merge(x, y);
    }
};

signed main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n,q;
  cin>>n>>q;
  vector<int64_t> prev(n), next(n);
  for (int i = 0; i < n; i++)
  {
    int a;
    cin>>a;
    prev[i] = a - i;
    next[i] = a + i;
  }
  Segment_Tree left(prev), right(next);
  while(q--)
  {
    int op;
    cin>>op;
    if ( op == 1 )
    {
      int idx, val;
      cin>>idx>>val;
      idx--;
      left.update(0, n - 1, 1, idx, val - idx);
      right.update(0, n - 1, 1, idx, val + idx);
    } else {
      int idx;
      cin>>idx;
      idx--;
      int64_t res_left = left.query(0, n - 1, 1, 0, idx) + idx;
      int64_t res_right = right.query(0, n - 1, 1, idx, n - 1) - idx;
#ifdef DEBUG
 printf("left %ld right %ld\n", res_left, res_right);
#endif
      printf("%ld\n", min(res_left, res_right));
    }
  }
}