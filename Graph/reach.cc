#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

// Reachable Nodes
// main idea - for some vertex we can OR mask of reachable nodes for all out-goimg nodes
using namespace std;

// results for various containers:
// unordered_set 218s
// set 500s
// bitset 618s
// primitive Bset 25s

struct Bset
{
  // 50000 bits is 6250 bytes but we need align on 32bit so 6252 * 8 = 50016 bits
  unsigned char b[6252];
  // int res;
  Bset()
  { reset(); }
  void reset()
  { memset(b, 0, sizeof(b)); }
  Bset &operator|=(const Bset &rhs)
  {
    const uint32_t *rp = (const uint32_t *)rhs.b;
    for ( uint32_t *p = (uint32_t *)b; p < (uint32_t *)(b + sizeof(b)); ++p, ++rp )
     *p |= *rp;
    return *this;
  }
  inline void set(int i)
  {
    int idx = i >> 3; // div 8
    int mask = i & 7;
    b[idx] |= 1 << mask;
  }
  inline bool test(int i)
  {
    int idx = i >> 3; // div 8
    int mask = i & 7;
    return b[idx] & (1 << mask);
  }
  int count()
  { int res = 0;
    for ( uint32_t *p = (uint32_t *)b; p < (uint32_t *)(b + sizeof(b)); ++p )
      res += __builtin_popcount(*p);
    return res;
  }
};

struct node
{
  int n;
  vector<int> edges;
  Bset s;
  // vector<int> in_edges;
  inline int degree() const
  {
    return edges.size();
  }
  // inline int in_degree() const
  // {
  //    return in_edges.size();
  // }
  void dump() const
  {
    printf("%d: ", n);
    for ( int e: edges )
      printf("%d ", e);
    printf("\n");
  }
  int visited = 0;
};

struct graph
{
  vector<node> nodes;
  graph(int N)
   : nodes(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
  }
  void dump() const
  {
    for ( auto &n: nodes )
     n.dump();
  }
  inline void add_edge(int a, int b)
  {
    node &n = nodes[a-1];
    n.edges.push_back(b-1);
//    node &m = nodes[b-1];
//    m.in_edges.push_back(a-1);
  }
  // based on https://neerc.ifmo.ru/wiki/index.php?title=%D0%98%D1%81%D0%BF%D0%BE%D0%BB%D1%8C%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%BE%D0%B1%D1%85%D0%BE%D0%B4%D0%B0_%D0%B2_%D0%B3%D0%BB%D1%83%D0%B1%D0%B8%D0%BD%D1%83_%D0%B4%D0%BB%D1%8F_%D0%BF%D0%BE%D0%B8%D1%81%D0%BA%D0%B0_%D1%86%D0%B8%D0%BA%D0%BB%D0%B0
  // complexity O(V + E)
  void dfs(int v)
  {
#ifdef DEBUG
printf("dfs %d:", v);
#endif
    if ( nodes[v].visited )
      return;
    nodes[v].s.set(v);
    for ( int u: nodes[v].edges )
    {
      dfs(u);
      nodes[v].s |= nodes[u].s;
    }
    nodes[v].visited = true;
  }
};

inline void printTime(const char *pfx)
{
#ifdef DEBUG
  printf("%s: %f\n", pfx, (float)clock()/CLOCKS_PER_SEC);
#endif
}

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  graph g(n);
  for ( int i = 0; i < m; ++i )
  {
    int a, b;
    cin>>a>>b;
    g.add_edge(a, b);
  }
#ifdef DEBUG
  g.dump();
#endif
  for ( int i = 0; i < n; ++i )
  {
    g.dfs(i);
    printf("%d ", g.nodes[i].s.count());
  }
  printf("\n");
  printTime("results");
#ifdef DEBUG
  g.dump_stat();
#endif
}
