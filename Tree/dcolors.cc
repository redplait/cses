#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

// solution for Distinct Colors
// main idea - for some vertex we can OR mask of colors from all children
// this can work fine with balanced trees however we have very skewed trees in tests so even simple DFS lead to
// stack exhaustion (for example on test 6)
// So we should use several dirty tricks like
// 1) emulate recursive DFS when processing chains with only one child
// 2) for such chains we can have only 1 Bset and test if color of currently processed node already inside it
// Another unpleasant observation - this is NOT binary trees. Proof: in test 5 node 2 has degreee 4
// so is_fork should fill list of non-visited children, not just 1 left & 1 right
// Results: 0.85s on test 10
using namespace std;

struct Bset
{
  static int s_cnt, s_mrg;
  static int s_size32;
  static void set_size(int s)
  {
    s_size32 = s >> 5; // / 32
    if ( s & 0x1f ) s_size32++;
    s_size32 <<= 2; // * 4
#ifdef DEBUG
 printf("set_size: s %d s_size32 %d\n", s, s_size32);
#endif
  }
  // 200000 bits is 25000 bytes
  unsigned char b[25000];
  // int res;
  Bset()
  { reset(); }
  void reset()
  { memset(b, 0, s_size32); }
  Bset &operator|=(const Bset &rhs)
  {
    const uint32_t *rp = (const uint32_t *)rhs.b;
    for ( uint32_t *p = (uint32_t *)b; p < (uint32_t *)(b + s_size32); ++p, ++rp )
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
  int merge(const Bset *rhs)
  { s_mrg++;
    int res = 0;
    const uint32_t *rp = (const uint32_t *)rhs->b;
    for ( uint32_t *p = (uint32_t *)b; p < (uint32_t *)(b + s_size32); ++p, ++rp )
    {
     *p |= *rp;
     res += __builtin_popcount(*p);
    }
    return res;
  }
  int count()
  { s_cnt++;
    int res = 0;
    for ( uint32_t *p = (uint32_t *)b; p < (uint32_t *)(b + s_size32); ++p )
      res += __builtin_popcount(*p);
    return res;
  }
};

int Bset::s_cnt = 0;
int Bset::s_mrg = 0;
int Bset::s_size32 = 25000;

struct node
{
#ifdef DEBUG
  int n;
#endif
  vector<int> edges;
  int color = -1;
  Bset *colors = nullptr; // only for non-leaf nodes
  int ncolors = 1;
  // vector<int> in_edges;
  inline int degree() const
  {
    return edges.size();
  }
#ifdef DEBUG
  void dump() const
  {
    printf("%d: ", n);
    for ( int e: edges )
      printf("%d ", e);
    printf("\n");
  }
#endif
  bool is_leaf(int parent, char *vis)
  {
    if ( leaf ) return true;
    if ( edges.empty() ) return (leaf = true);
    for ( auto v: edges )
    {
      if ( v == parent ) continue;
      if ( vis[v] ) continue;
      return false;
    }
    return (leaf = true);
  }
  bool is_fork(int parent, vector<int> &r, char *vis)
  {
    if ( edges.empty() )
    {
      leaf = 1;
      return false;
    }
    int res = 0;
    for ( auto v: edges )
    {
      if ( v == parent ) continue;
      if ( vis[v] ) continue;
      r.push_back(v);
      res++;
    }
    if ( !res )
    {
      leaf = 1;
      return false;
    }
    return res >= 2;
  }
  char leaf = 0;
};

struct graph
{
  vector<node> nodes;
  int alloced = 0;
  int removed = 0;
  int dfs_cnt = 0;
  int both_leaves = 0;
  char *visited;
  graph(int N)
   : nodes(N)
  {
#ifdef DEBUG
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
#endif
    visited = (char *)calloc(N, 1);
  }
#ifdef DEBUG
  void dump() const
  {
    for ( auto &n: nodes ) n.dump();
  }
#endif
  inline void add_edge(int a, int b)
  {
    node &n = nodes[a-1];
    n.edges.push_back(b-1);
    node &m = nodes[b-1];
    m.edges.push_back(a-1);
  }
  // based on https://neerc.ifmo.ru/wiki/index.php?title=%D0%98%D1%81%D0%BF%D0%BE%D0%BB%D1%8C%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%BE%D0%B1%D1%85%D0%BE%D0%B4%D0%B0_%D0%B2_%D0%B3%D0%BB%D1%83%D0%B1%D0%B8%D0%BD%D1%83_%D0%B4%D0%BB%D1%8F_%D0%BF%D0%BE%D0%B8%D1%81%D0%BA%D0%B0_%D1%86%D0%B8%D0%BA%D0%BB%D0%B0
  // complexity O(V + E)
  void dfs(int v, int p)
  {
#ifdef DEBUG
printf("dfs(%d,%d) degree %d color %d\n", v, p, (int)nodes[v].edges.size(), nodes[v].color);
#endif
    if ( visited[v] ) return;
    int old_v = v, old_p = p;
    dfs_cnt++;
    stack<int> st;
    int curr_size = 0;
    Bset *curr = nullptr;
    while(1)
    {
      vector<int> children;
      visited[v] = 1;
      st.push(v);
      if ( nodes[v].is_fork(p, children, visited) )
      {
#ifdef DEBUG
printf("fork %d parent %d, children %d\n", v, p, (int)children.size());
#endif
        if ( children.size() == 2 )
        {
          int l = children[0]; int r = children[1];
          nodes[l].is_leaf(v, visited);
          nodes[r].is_leaf(v, visited);
          // check if both children are leaves
          if ( nodes[l].leaf && nodes[r].leaf )
          {
#ifdef DEBUG
printf("both %d and %d are leaves\n", l, r);
#endif
            visited[l] = visited[r] = 1;
            both_leaves++;
            curr = new Bset(); alloced++;
            curr->set(nodes[l].color);
            curr_size = 1;
            if ( nodes[r].color != nodes[l].color )
            {
              curr->set(nodes[r].color);
              curr_size = 2;
            }
            break;
          }
          // left node is leaf ?
          if ( nodes[l].leaf )
          {
            visited[l] = 1;
            dfs(r, v);
            curr = nodes[r].colors;
#ifdef DEBUG
printf("l %d curr %p\n", l, curr);
#endif
            curr_size = nodes[r].ncolors;
            if ( !curr->test(nodes[l].color) )
            { curr->set(nodes[l].color); curr_size++; }
            break;
          }
          // right node is leaf ?
          if ( nodes[r].leaf )
          {
            visited[r] = 1;
            dfs(l, v);
            curr = nodes[l].colors;
#ifdef DEBUG
printf("r %d curr %p\n", r, curr);
#endif
            curr_size = nodes[l].ncolors;
            if ( !curr->test(nodes[r].color) )
            { curr->set(nodes[r].color); curr_size++; }
            break;
          }
          // both not leaves
          dfs(l, v); dfs(r, v);
#ifdef DEBUG
printf("both %d & %d, ncolors %d %d\n", l, r, nodes[l].colors->count(), nodes[r].colors->count());
#endif
          curr = nodes[l].colors; curr_size = nodes[l].ncolors;
          if ( nodes[r].colors )
          {
            curr_size = curr->merge(nodes[r].colors);
            delete nodes[r].colors; removed++;
          }
          break;
        }
#ifdef DEBUG
        printf("degree of %d is %d\n", v, (int)children.size());
#endif
        curr = new Bset(); alloced++;
        curr->set(nodes[v].color);
        curr_size = 1;
        for ( int c: children )
        {
          if ( visited[c] ) continue;
          if ( nodes[c].is_leaf(v, visited) )
          {
            if ( !curr->test(nodes[c].color) )
            { curr->set(nodes[c].color); curr_size++; }
            visited[c] = 1;
            continue;
          }
          dfs(c, v);
          if ( nodes[c].colors )
          {
            curr_size = curr->merge(nodes[c].colors);
            delete nodes[c].colors; removed++;
          }
        }
        break;
      }
      if ( nodes[v].leaf )
      {
        curr = new Bset(); alloced++;
        curr->set(nodes[v].color);
#ifdef DEBUG
 printf("last leaf %d, color %d, colors %d\n", v, nodes[v].color, curr->count());
#endif
        curr_size = 1;
        break;
      }
      p = v;
      v = children[0];
    }
#ifdef DEBUG
printf("dfs %d curr %p curr_size %d real %d\n", old_v, curr, curr_size, curr->count());
#endif
    // unwind stack
    while( !st.empty() )
    {
      int s = st.top(); st.pop();
      if ( !curr->test(nodes[s].color) )
      { curr->set(nodes[s].color); curr_size++; }
      nodes[s].ncolors = curr_size;
    }
    if ( old_p == -1 )
    { delete curr; removed++; }
    else
      nodes[old_v].colors = curr;
  }
};

inline void printTime(const char *pfx)
{
#ifdef TIME
  printf("%s: %f\n", pfx, (float)clock()/CLOCKS_PER_SEC);
#endif
}

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n;
  cin>>n;
  graph *g = new graph(n);
  map<int, int> color_remap;
  int color;
  int cidx = 0;
  for ( int i = 0; i < n; ++i )
  {
    cin>>color;
    auto it = color_remap.find(color);
    if ( it == color_remap.end() )
    {
      g->nodes[i].color = cidx;
      color_remap[color] = cidx;
      cidx++;
    } else {
      g->nodes[i].color = it->second;
    }
  }
#ifdef DEBUG
printf("colors %ld %d\n", color_remap.size(), cidx);
#endif
  Bset::set_size(cidx);
  color_remap.clear();
  // read edges
  for ( int i = 0; i < n - 1; i++ )
  {
    int a, b;
    cin>>a>>b;
    g->add_edge(a, b);
  }
#ifdef DEBUG
  g->dump();
#endif
  for ( int i = 0; i < n; ++i )
  {
    if ( !g->visited[i] ) g->dfs(i, -1);
    printf("%d ", g->nodes[i].ncolors);
  }
  printf("\n");
#ifdef TIME
  printTime("results");
  // calc amount of leaves
  int lcnt = count_if(g->nodes.begin(), g->nodes.end(), [](const node &v) { return v.leaf; });
  printf("alloced %d removed %d dfs %d both_leaves %d leaves %d conts %d merges %d Bsize %d\n",
    g->alloced, g->removed, g->dfs_cnt, g->both_leaves, lcnt, Bset::s_cnt, Bset::s_mrg, Bset::s_size32);
#endif
}
