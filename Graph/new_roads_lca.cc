#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <vector>
#include <unordered_set>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

using namespace std;

// solution for New Roads Queries using LCA
// results 0.87s on test 6

struct edge
{
  int a, b, day;
  char visited = 0;
};

struct connected;

struct node
{
  int n;
  connected *c = nullptr;
};

struct connected
{
  int root; // edge idx
  unordered_set<node *> nodes;
  void merge(connected *rhs)
  {
    for ( auto n: rhs->nodes )
    {
      n->c = this;
      nodes.insert(n);
    }
    delete rhs;
  }
  connected &operator+=(connected &rhs)
  {
    for ( auto n: rhs.nodes )
    {
      n->c = this;
      nodes.insert(n);
    }
    return *this;
  }
};

struct graph
{
  vector<node> nodes;
  vector<edge> edges;
  unordered_set<connected *> comps;
  int N;
  int edge_idx = 0;
  graph(int n)
   : nodes(n), N(n)
  {
    edges.reserve(N);
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
  }
  int new_edge(int a, int b, int day)
  {
    edges.push_back( { a, b, day } );
    return N + edge_idx++;
  }
  inline void add_edge(int a, int b, int days)
  {
    node &an = nodes[a-1];
    node &bn = nodes[b-1];
    if ( !an.c && !bn.c ) // just edge between some unconnected nodes
    {
      connected *cc = new connected();
      comps.insert(cc);
      an.c = bn.c = cc;
      cc->nodes.insert(&an); cc->nodes.insert(&bn);
      cc->root = new_edge(a - 1, b - 1, days + 1);
      return;
    }
    // check if we try add edge to the same connection set
    if ( an.c == bn.c )
      return;
    if ( !an.c ) // add node an to connection set in bn
    {
      int root = new_edge(an.n, bn.c->root, days + 1);
      an.c = bn.c;
      bn.c->nodes.insert(&an);
      bn.c->root = root;
      return;
    }
    if ( !bn.c) // add node bn to connection set in an
    {
      int root = new_edge(bn.n, an.c->root, days + 1);
      bn.c = an.c;
      an.c->nodes.insert(&bn);
      an.c->root = root;
      return;
    }
    // merge two connection sets
    int root = new_edge(an.c->root, bn.c->root, days + 1);
    an.c->root = bn.c->root = root;
    if ( an.c->nodes.size() > bn.c->nodes.size() )
    {
      comps.erase(bn.c);
      an.c->merge(bn.c);
    } else {
      comps.erase(an.c);
      bn.c->merge(an.c);
    }
  }
  // LCA data
  int exp_max;
  vector<int> height;
  vector< vector<int> > anc;
  void dfs_lca(int node, int parent, int h = 0)
  {
#ifdef DEBUG
 printf("dfs %d h %d\n", node, h);
#endif
    height[node] = h;
    anc[node][0] = parent;
    if ( node < N ) return;
    edge &ed = edges[node-N];
    ed.visited = 1;
    dfs_lca( ed.a, node, h + 1 );
    dfs_lca( ed.b, node, h + 1 );
  }
  inline int lift(int node, int up)
  {
    if ( height[node] < up ) return -1;
    int res = node;
    for ( int h = 0; up; h++ )
    {
      if ( up & 1 ) res = anc[res][h];
      up >>= 1;
    }
    return res;
  }
  int find_LCA(int u, int v)
  {
#ifdef DEBUG
    int old_u = u, old_v = v;
#endif
    if ( height[u] < height[v] )
      v = lift(v, height[v] - height[u]);
    else if ( height[v] < height[u] )
      u = lift(u, height[u] - height[v]);
#ifdef DEBUG
 printf("lca(%d depth %d, %d depth %d) u %d v %d\n", 1+old_u, height[old_u], 1+old_v, height[old_v], u, v);
#endif
    if ( u == v ) return v;
    for ( int e = exp_max; e >= 0; e-- )
    {
      if ( anc[u][e] != anc[v][e] )
      {
        u = anc[u][e];
        v = anc[v][e];
      }
    }
    return anc[u][0];
  }
  void make_LCA()
  {
    int total = N + edge_idx;
    exp_max = 0;
    int k = 1;
    while( k < total )
    {
      k <<= 1;
      exp_max++;
    }
#ifdef DEBUG
 printf("exp_max %d\n", exp_max);
#endif 
    height.resize(total, 0);
    anc.resize(total);
    for ( auto &u: anc ) u.resize(exp_max + 1, -1);
    for ( auto c: comps )
    {
      auto &ed = edges[c->root - N];
      if ( !ed.visited ) {
#ifdef DEBUG
 printf("dfs_lca on %d\n", c->root - N);
#endif
        dfs_lca(c->root, -1);
      }
    }
#ifdef DEBUG
    for ( int i = 0; i < edge_idx; i++ )
      if ( !edges[i].visited ) printf("BUG: edge %d %d <-> %d not visited\n", i + N, edges[i].a, edges[i].b);
    for ( int i = 0; i < total; i++ )
      printf("%d h %d\n", i, height[i]);
#endif
    // finalyze
    for ( int e = 1; e <= exp_max; e++ )
    {
      for ( int node = 0; node < total; node++ )
      {
        if ( anc[node][e-1] != -1 ) anc[node][e] = anc[ anc[node][e-1] ][e - 1];
      }
    }
  }
  int query(int a, int b)
  {
    if ( a == b ) return 0;
    a--; b--;
    // check if they are connected
    if ( !nodes[a].c || !nodes[b].c || nodes[a].c != nodes[b].c ) return -1;
    int root = find_LCA(a, b);
    if ( root == -1 ) return -1;
    if ( root < N )
    { 
      printf("BUG: a %d b %d root %d\n", a+1, b+1, root);
      return -1;
    }
    return edges[root - N].day;
  }
};

inline void printTime(const char *pfx)
{
#ifdef TIME
  printf("%s: %f\n", pfx, (float)clock()/CLOCKS_PER_SEC);
#endif
}

int main(int argc, char **argv)
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m, q;
  cin>>n>>m>>q;
  graph g(n);
  for ( int i = 0; i < m; ++i )
  {
    int a, b;
    cin>>a>>b;
    g.add_edge(a, b, i);
  }
  printTime("make connections");
#ifdef DEBUG
  printf("edges %d\n", g.edge_idx);
#endif
#ifdef TEST
  ifstream ti;
  if ( argc > 1 ) ti.open(argv[1]);
#endif
  g.make_LCA();
  printTime("LCA");
  for ( int i = 0; i < q; ++i )
  {
    int a, b;
    cin>>a>>b;
    int res = g.query(a, b);
    printf("%d\n", res);
#ifdef TEST
    if ( ti.is_open() )
    {
      int mb; ti>>mb;
      if ( mb != res ) printf("WRONG %d %d: %d vs %d\n", a, b, res, mb);
    }
#endif
  }
  printTime("results");
}
