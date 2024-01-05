#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("O3,unroll-loops")

// solution for Subtree Queries
// main idea is to build segment tree but bcs original tree is not binary we need to detect nodes orders
// and later for query/updates use in_order as start index and out_order as end
// Results: 0.34s
using namespace std;

template <typename T>
struct stq
{
  int N;
  int timer = 0;
  vector<vector<int> > adj;
  vector<int> in_order, out_order;
  vector<T> v, dfstree, segtree;
  char *visited;
  stq(int n): N(n), adj(n), in_order(n), out_order(n), v(n), segtree(4 * n)
  {
    dfstree.reserve(n);
    visited = (char *)calloc(N, 1);
  }
  // 0 if leaf, 1 if stem
  int node_kind(int v, vector<int> &desc)
  {
    if ( adj[v].empty() ) return 0;
    int res = 0;
    for ( auto c: adj[v] )
    {
      if ( visited[c] ) continue;
      res++;
      desc.push_back(c);
    }
    return res;
  }
  // tree can have very long stem so try to process such chains using std::stack
  void dfs(int v)
  {
    if ( visited[v] ) return;
    stack<int> st;
    while(1)
    {
      vector<int> children;
      visited[v] = 1;
      in_order[v] = timer++;
      dfstree.push_back(v);
      st.push(v);
      int kind = node_kind(v, children);
      if ( !kind ) break;
      if ( kind > 1 )
      {
        for ( int c: children ) dfs(c);
        break;
      }
      v = children[0];
    }
    // process stack
    while( !st.empty() )
    {
      int s = st.top(); st.pop();
      out_order[s] = timer - 1;
    }
  }
  void read()
  {
    // read values of each node
    for ( int i = 0; i < N; i++ ) cin>>v[i];
    // read edges
    for ( int i = 0; i < N - 1; i++ )
    {
      int a, b;
      cin>>a>>b;
      a--, b--;
      adj[a].push_back(b); adj[b].push_back(a);
    }
    // now calc orders of nodes
    for ( int i = 0; i < N; i++ )
      if ( !visited[i] ) dfs(i);
#ifdef DEBUG
  for ( int i = 0; i < N; i++ )
 printf("%d in %d out %d\n", i, in_order[i], out_order[i]);
#endif
    // fill dfstree
    for ( int i = 0; i < N; i++ )
    {
      dfstree[i] = v[dfstree[i]];
#ifdef DEBUG
 printf("dfs[%d] %ld\n", i, dfstree[i]);
#endif
    }
    // make segment tree
    build(0, N - 1, 1);
    // cleanup
    adj.clear();
    free(visited);
  }
  // segment tree
  inline T merge(T a, T b) 
  {
    return a + b;
  }
  void build(int s, int e, int root)
  {
    if (s == e)
    {
	    segtree[root] = dfstree[s];
	    return;
    }
    int mid = (s + e) / 2;
    build(s, mid, 2 * root);
    build(mid + 1, e, 2 * root + 1);

    // self work
    segtree[root] = merge(segtree[2 * root], segtree[2 * root + 1]);
  }
  void update(int s, int e, int root, int idx, T val)
  {
    if (s == e) {
     segtree[root] = val;
     return;
    }
    int mid = (s + e) / 2;
    if (mid >= idx) {
      update(s, mid, 2 * root, idx, val);
    }
    else {
      update(mid + 1, e, 2 * root + 1, idx, val);
    }
    // self work
    segtree[root] = merge(segtree[2 * root], segtree[2 * root + 1]);
  }
  T query(int s, int e, int root, int l, int r)
  {
    // completely out of range
    if (s > r || e < l) {
      return 0;
    }
    // completely in range
    if (s >= l && e <= r) {
      return segtree[root];
    }
    // partial overlap
    int mid = (s + e) / 2;
    T x = query(s, mid, 2 * root, l, r);
    T y = query(mid + 1, e, 2 * root + 1, l, r);
    return merge(x, y);
  }
  void update(int s, T val)
  {
    s--;
    int index = in_order[s];
    update(0, N-1, 1, index, val);
  }
  T query(int x)
  {
    x--;
    return query(0, N - 1, 1, in_order[x], out_order[x]);
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, q;
  cin>>n>>q;
  stq<int64_t> t(n);
  t.read();
  while(q--)
  {
    int what;
    cin>>what;
    if ( 1 == what )
    {
      int idx;
      long long v;
      cin>>idx>>v;
      t.update(idx, v);
    } else {
      int x;
      cin>>x;
      printf("%ld\n", t.query(x));
    }
  }
}