#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <unordered_set>
#include <queue>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

// Reachability Queries
// at this time graph has cycles. Effectively all nodes in cycle must have the same bitmask
// we can achieve this just sharing common single mask for several nodes
// so now node has pointer to mask and we need 4 pass:
// 1) to find cycles and assing common mask for each cycle
// 2) to assign masks for nodes outside of cycles
// 3) fill bitmasks
// 4) last pass to fill bitmasks again for nodes outside of cycles bcs at pass 3 masks for cycles was not completed yet
// also cycles should be merged. Lets consider graph
//    1   5     If we start search from node 1 then we put in stack 2
//   / \ / \    node 2 will detect cycle after visiting 5 6 7 (or 7 6 5) -> 2 and assign some mask to all this nodes
//  4   2   6   then return to 3 4 and at 1 another cycle will be detected
//   \/  \ /    If we will give new fresh mask to 2 then cycle 2 5 6 7 will be unlinked
//   3    7     so when processing 1 2 3 4 we need to collect mask from all this nodes before requiring new one
using namespace std;

struct Bset
{
  // 50000 bits is 6250 bytes but we need align on 32bit so 6252 * 8 = 50016 bits
  unsigned char b[6252];
  // int res;
  Bset()
  { reset(); }
  void reset()
  { memset(b, 0, sizeof(b)); }
  void merge(const Bset &rhs)
  {
    const uint32_t *rp = (const uint32_t *)rhs.b;
    for ( uint32_t *p = (uint32_t *)b; p < (uint32_t *)(b + sizeof(b)); ++p, ++rp )
     *p |= *rp;
  }
  Bset &operator|=(const Bset &rhs)
  {
    merge(rhs);
    return *this;
  }
  inline void set(int i)
  {
    int idx = i >> 3; // div 8
    int mask = i & 7;
    b[idx] |= 1 << mask;
  }
  inline bool test(int i) const
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
#ifdef TEST
  vector<int> in_edges;
#endif
  Bset *s = nullptr;
  int s_idx = 0; // bitmask index
  char color = 0; 
  // vector<int> in_edges;
  inline int degree() const
  {
    return edges.size();
  }
#ifdef TEST  
  inline int in_degree() const
  {
    return in_edges.size();
  }
#endif  
  void dump() const
  {
    printf("%d", n);
    if ( s_idx )
      printf(" cycle %d", s_idx);
    printf(": ");
    for ( int e: edges )
      printf("%d ", e);
    printf("\n");  
#ifdef TEST
  printf(" in:");
  for ( int e: in_edges )
      printf(" %d", e);
  printf("\n");
#endif
  }
  // very slowly
  void dump(int N) const
  {
    printf("%d", n);
    if ( s_idx )
      printf(" cycle %d", s_idx);
    printf(": ");
    for ( int i = 0; i < N; ++i )
      if ( s->test(i) )
       printf("%d ", i);
    printf("\n");
  }
};

struct graph
{
  vector<node> nodes;
  vector<Bset> bmasks;
  int bm_idx = 0; 
  graph(int N)
   : nodes(N),
     bmasks(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
  }
  Bset *get_mask()
  {
    if ( bm_idx < bmasks.size() )
      return &bmasks[bm_idx++];
    printf("bug - no next mask, idx %d", bm_idx);
    exit(3);
  }
  void dump() const
  {
    for ( auto &n: nodes )
     n.dump();
  }
  void dump(int N) const
  {
    for ( auto &n: nodes )
     n.dump(N);
  }
  inline void add_edge(int a, int b)
  {
    node &n = nodes[a-1];
    n.edges.push_back(b-1);
#ifdef TEST
    node &m = nodes[b-1];
    m.in_edges.push_back(a-1);
#endif
  }
  // based on https://neerc.ifmo.ru/wiki/index.php?title=%D0%98%D1%81%D0%BF%D0%BE%D0%BB%D1%8C%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%BE%D0%B1%D1%85%D0%BE%D0%B4%D0%B0_%D0%B2_%D0%B3%D0%BB%D1%83%D0%B1%D0%B8%D0%BD%D1%83_%D0%B4%D0%BB%D1%8F_%D0%BF%D0%BE%D0%B8%D1%81%D0%BA%D0%B0_%D1%86%D0%B8%D0%BA%D0%BB%D0%B0
  // complexity O(V + E)
  void dfs2(int v)
  {
    if ( nodes[v].color == 3 )
      return;
    nodes[v].color = 3;
    nodes[v].s->set(v);
    for ( int u: nodes[v].edges )
    {
      if ( v == u ) // cycle to this node itself?
        continue;
      nodes[v].s->set(u);  
      if ( nodes[u].color != 3 )
        dfs2(u);
      nodes[v].s->merge(*nodes[u].s);
    }
  }
  void dfs1(int v, vector<int> &s)
  {
#ifdef DEBUG
printf("dfs %d\n", v);
#endif
    if ( nodes[v].color == 2 )
      return;
    nodes[v].color = 1;
    for ( int u: nodes[v].edges )
    {
      if ( v == u ) // cycle to this node itself?
         continue;
      if ( !nodes[u].color )
      {
#ifdef DEBUG
printf("u %d\n", u);
#endif
        s.push_back(v);
        dfs1(u, s);
        s.pop_back();
      }
/* Let assume we have cycle 1 2 3 1
   dfs v=1 stack empty
    dfs v=2 stack 1
     dfs v=3 stack 1 2 u=1. now we need to add to [u].cycled v and all nodes from stack until u
 */
      if ( nodes[u].color == 1 && !nodes[u].s )
      {
#ifdef DEBUG
 printf("found cycle v %d from %d\n", v, u);
 printf("stack:");
 for ( auto si = s.rbegin(); si != s.rend(); ++si ) printf(" %d", *si);
 printf("\n");
#endif
        Bset *bm = nodes[u].s;
        int s_idx = nodes[u].s_idx;
        // try get mask from v
        if ( !bm )
        {
          bm = nodes[v].s;
          s_idx = nodes[v].s_idx;
        }
        // still no mask? iterate over stack until u
        if ( !bm )
        {
          for ( auto si = s.rbegin(); si != s.rend(); ++si )
          {
            if ( *si == u )
              break;
            bm = nodes[*si].s;
            s_idx = nodes[*si].s_idx;
            if ( bm )
              break;
          }
        }
        if ( !bm )
        {
          bm = get_mask();
          s_idx = bm_idx;
 #ifdef DEBUG
  printf("allocated new mask idx %d\n", s_idx);
 #endif
        }
        nodes[v].s = bm;
        nodes[v].s_idx = s_idx;
        for ( auto si = s.rbegin(); si != s.rend(); ++si )
        {
#ifdef DEBUG
 printf("add %d to v %d mask ids %d\n", *si, u, s_idx);
#endif
          nodes[*si].s = bm;
          nodes[*si].s_idx = s_idx;
          if ( *si == u )
            break;
        }
      }
    }
    nodes[v].color = 2;
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
  int n, m, q;
  cin>>n>>m>>q;
  graph g(n);
  for ( int i = 0; i < m; ++i )
  {
    int a, b;
    cin>>a>>b;
    g.add_edge(a, b);
  }
  vector<int> s;
  // find cycles
  for ( int i = 0; i < n; ++i )
  {
    if ( g.nodes[i].color )
      continue;
    s.clear();
    g.dfs1(i, s);
  }
#ifdef DEBUG
  printf("used masks: %d\n", g.bm_idx);
#endif  
  printTime("cycles");
  for ( int i = 0; i < n; ++i )
  {
    if ( g.nodes[i].s )
      continue;
    g.nodes[i].s = g.get_mask();
  }
  // fill
  for ( int i = 0; i < n; ++i )
  {
    g.dfs2(i);
  }
  // reset colors
  for ( int i = 0; i < n; ++i )
  {
    g.nodes[i].color = 0;
  }
  for ( int i = 0; i < n; ++i )
  {
    if ( !g.nodes[i].s_idx)
      g.dfs2(i);
  }
  printTime("cached");
#ifdef DEBUG
  g.dump(n);
#endif
  // process queries
#ifdef TEST
  ifstream ti("test_output.txt");
  string as;
#endif
  for ( int i = 0; i < q; ++i )
  {
    int a, b;
    cin>>a>>b;
    a--;
    b--;
    int my_res;
#ifdef TEST
    ti>>as;
    int test_res = 0;
    if ( as == "YES" )
      test_res = 1;
#endif
    if ( g.nodes[a].s->test(b) )
    {
#ifndef TEST
      puts("YES");
#endif
      my_res = 1;
    } else {
#ifndef TEST
      puts("NO");
#endif
      my_res = 0;
    }
#ifdef TEST
    if ( test_res == my_res )
      continue;
    printf("wrong answer %d must be %s for %d %d\n", my_res, as.c_str(), a, b);
    g.nodes[a].dump();
    g.nodes[a].dump(n);
    std::unordered_set<int> visited;
    node *root = &g.nodes[a];
    queue<node *> st;
    st.push(root);   
    while(!st.empty() )
    {
      node *curr = st.front(); 
      st.pop();
      if ( !root->s->test(curr->n) )
      {
        printf("no curr %d in root %d\n", curr->n, root->n);
        curr->dump();
        curr->dump(n);
        break;
      }
      visited.insert(curr->n);
      for ( int i: curr->edges)
      {
        if ( curr->s_idx )
        {
          if ( !curr->s->test(i) )
          {
            printf("no child %d in %d\n", i, curr->n);
            curr->dump();
            curr->dump(n);
            return 2;
          }
        }
        auto ei = visited.find(i);
        if ( ei != visited.end() )
          continue;
        auto e = &g.nodes[i];
        st.push(e);
        e->dump();
      }
    }
    return 1;
#endif
  }
  printTime("results");
}
