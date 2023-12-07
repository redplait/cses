#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <map>
#include <vector>

// solution for Range Updates and Sums
// Segment_Tree ripped from hotel_q.cc + 2 lazy trees
using namespace std;
typedef int64_t item;

// probably better to make it as template class but I am too lazy
struct Segment_Tree
{
    vector<item> segtree;
    vector<item> lazy1, lazy2;
    Segment_Tree(vector<int> &a)
    {
      int n = (int)a.size();
      segtree = vector<item>(4 * n);
      lazy1 = vector<item>(4 * n, 0);
	   lazy2 = vector<item>(4 * n, 0);
      build(a, 0, n - 1, 1);
    }
    item merge(item a, item b)
    {
      return a + b; // bcs this is sum
    }
    void build(vector<int> &a, int s, int e, int root)
    {
      if (s == e) {
       segtree[root] = item{ a[s] };
       return;
      }
      int mid = (s + e) / 2;
      build(a, s, mid, 2 * root);
      build(a, mid + 1, e, 2 * root + 1);

      segtree[root] = merge(segtree[2 * root], segtree[2 * root + 1]);
    }
    void propagate(int root, int s, int e)
	 {
      int width = e - s + 1;
		if (lazy1[root] != 0)
		{
			segtree[root] += width * lazy1[root];
			if (s != e)
			{
				if (lazy2[2 * root] != 0) lazy2[2 * root] += lazy1[root];
				else lazy1[2 * root] += lazy1[root];
				if (lazy2[2 * root + 1] != 0) lazy2[2 * root + 1] += lazy1[root];
				else lazy1[2 * root + 1] += lazy1[root];
			}
		}
		if (lazy2[root] != 0)
		{
			segtree[root] = width * lazy2[root];

			if (s != e)
			{
				if (lazy2[2 * root] != 0)
					lazy2[2 * root] = lazy2[root];
				else {
					lazy2[2 * root] = lazy2[root];
					lazy1[2 * root] = 0;
				}

				if (lazy2[2 * root + 1] != 0)
					lazy2[2 * root + 1] = lazy2[root];
				else {
					lazy2[2 * root + 1] = lazy2[root];
					lazy1[2 * root + 1] = 0;
				}
			}
		}
		lazy1[root] = 0;
		lazy2[root] = 0;
	}

    void inc(int s, int e, int root, int l, int r, int64_t val)
    {
      propagate(root, s, e);
      if (s > r || e < l)
			return;

		if (s >= l && e <= r)
		{
			// make updates in the current node
			int width = e - s + 1;
			segtree[root] += width * val;
			if (s != e) // lazy inc
			{
				if (lazy2[2 * root] != 0) lazy2[2 * root] += val;
				else lazy1[2 * root] += val;
				if (lazy2[2 * root + 1] != 0) lazy2[2 * root + 1] += val;
				else lazy1[2 * root + 1] += val;
			}
			return;
		}

		int mid = (s + e) / 2;
		inc(s, mid, 2 * root, l, r, val);
		inc(mid + 1, e, 2 * root + 1, l, r, val);
		segtree[root] = merge(segtree[2 * root], segtree[2 * root + 1]);
    }
    void set(int s, int e, int root, int l, int r, int64_t val)
    {
      propagate(root, s, e);
		if (s > r || e < l)
			return;
		if (s >= l && e <= r)
		{
			int width = e - s + 1;
			segtree[root] = width * val;
			if (s != e) // lazy update
			{
				if (lazy2[2 * root] != 0) lazy2[2 * root] = val;
				else
				{
					lazy1[2 * root] = 0;
					lazy2[2 * root] = val;
				}

				if (lazy2[2 * root + 1] != 0) lazy2[2 * root + 1] = val;
				else
				{
					lazy1[2 * root + 1] = 0;
					lazy2[2 * root + 1] = val;
				}
			}
			return;
		}

		int mid = (s + e) / 2;
		set(s, mid, 2 * root, l, r, val);
		set(mid + 1, e, 2 * root + 1, l, r, val);
		segtree[root] = merge(segtree[2 * root], segtree[2 * root + 1]);
    }
    item query(int s, int e, int root, int l, int r) {  //query with root index set to 1
      propagate(root, s, e);
	  // completely out of range
	  if (s > r || e < l)
	    return 0;
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
    int64_t x;
    int op, a, b;
    cin>>op>>a>>b;
    switch(op)
    {
      case 1:
       cin>>x;
       st.inc(0, n - 1, 1, a - 1, b - 1, x);
       break;
      case 2:
       cin>>x;
       st.set(0, n - 1, 1, a - 1, b - 1, x);
       break;
      case 3:
       printf("%ld\n", st.query(0, n - 1, 1, a - 1, b - 1));
       break;
      default: printf("unknown op %d\n", op);
       return 1;
    }
  }
}