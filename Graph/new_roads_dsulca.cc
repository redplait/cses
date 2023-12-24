#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

using namespace std;

// solution for New Roads Queries using DSU & LCA
// Results with LCA(N + edge_idx) 0.87s on test 6
// We can reduce size of LCA tree - just store initial edge in each node and include to LCA tree only edges
// Results with LCA(edge_idx) 0.77s on test 6
// Another observation is that we can use fixed-size DSU instead of merging sets
// while merging always choose lesser value. root edge of all connected components stored in dsu_roots
// new result is 0.48s on test 3

struct edge
{
  int a, b, day;
  char visited = 0;
};

struct connected;

struct node
{
  int n;
  int edge = -1;
};

struct graph
{
  vector<node> nodes;
  vector<edge> edges;
  vector<int> dsu;
  vector<int> dsu_roots; // edge index
  int comps = 0;
  int N;
  int edge_idx = 0;
  graph(int n)
   : nodes(n), dsu(n), dsu_roots(n, -1), N(n)
  {
    edges.reserve(N);
    for ( int i = 0; i < N; ++i )
    {
      nodes[i].n = i;
      dsu[i] = i;
    }
  }
  int find_dsu(int a)
  {
    if ( dsu[a] == a ) return a;
    return (dsu[a] = find_dsu(dsu[a]));
  }
  int new_edge(int a, int b, int day)
  {
    edges.push_back( { a, b, day } );
    return edge_idx++;
  }
  inline void add_edge(int a, int b, int days)
  {
    a--; b--;
    node &an = nodes[a];
    node &bn = nodes[b];
    int a_dsu = find_dsu(a);
    int b_dsu = find_dsu(b);
    int root = -1;
#define PATCHR dsu_roots[a_dsu] = dsu_roots[b_dsu] = root; \
    if ( a_dsu > b_dsu ) swap(a_dsu, b_dsu);  \
    dsu[b_dsu] = a_dsu; return;

    if ( an.edge == -1 && bn.edge == -1 ) // just edge between some unconnected nodes
    {
      root = new_edge(-1, -1, days + 1);
      comps++;
      an.edge = bn.edge = root;
      PATCHR
    }
    // check if we try add edge to the same connection set
    if ( a_dsu == b_dsu )
      return;
    if ( an.edge == -1 ) // add node an to connection set in bn
    {
      root = new_edge(-1, dsu_roots[b_dsu], days + 1);
      an.edge = root;
      PATCHR
    }
    if ( bn.edge == -1 ) // add node bn to connection set in an
    {
      root = new_edge(-1, dsu_roots[a_dsu], days + 1);
      bn.edge = root;
      PATCHR
    }
    // merge two connection sets
    root = new_edge(dsu_roots[a_dsu], dsu_roots[b_dsu], days + 1);
    comps--;
    PATCHR
  }
  // LCA data
  int exp_max;
  vector<int> height;
  vector< vector<int> > anc;
  void dfs_lca(int node, int parent, int h = 0)
  {
    height[node] = h;
    anc[node][0] = parent;
    edge &ed = edges[node];
    ed.visited = 1;
    if ( ed.a != -1 ) dfs_lca( ed.a, node, h + 1 );
    if ( ed.b != -1 ) dfs_lca( ed.b, node, h + 1 );
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
    exp_max = 0;
    int k = 1;
    while( k < edge_idx )
    {
      k <<= 1;
      exp_max++;
    }
#ifdef DEBUG
 printf("exp_max %d\n", exp_max);
#endif 
    height.resize(edge_idx, 0);
    anc.resize(edge_idx);
    for ( auto &u: anc ) u.resize(exp_max + 1, -1);
    for ( int i = edge_idx - 1; i >= 0; --i )
    {
      auto &ed = edges[i];
      if ( !ed.visited ) {
#ifdef DEBUG
 printf("dfs_lca on %d\n", i);
#endif
        dfs_lca(i, -1);
      }
    }
#ifdef DEBUG
    for ( int i = 0; i < edge_idx; i++ )
      if ( !edges[i].visited ) printf("BUG: edge %d %d <-> %d not visited\n", i, edges[i].a, edges[i].b);
    for ( int i = 0; i < edge_idx; i++ )
      printf("%d h %d\n", i, height[i]);
#endif
    // finalyze
    for ( int e = 1; e <= exp_max; e++ )
    {
      for ( int node = 0; node < edge_idx; node++ )
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
    int a_dsu = find_dsu(a);
    int b_dsu = find_dsu(b);
#ifdef DEBUG
 printf("a %d a_dsu %d b %d b_dsu %d dsu_roots[a] %d dsu_roots[b] %d\n", a+1, a_dsu, b+1, b_dsu, dsu_roots[a_dsu], dsu_roots[b_dsu]);
#endif
    if ( a_dsu != b_dsu ) return -1; // not connected
    int ea = nodes[a].edge,
        eb = nodes[b].edge;
    if ( -1 == ea || -1 == eb ) return -1;
    int root = find_LCA(ea, eb);
    if ( root == -1 ) return -1;
    if ( root >= edge_idx )
    {
      printf("BUG: a %d b %d root %d\n", a+1, b+1, root);
      return -1;
    }
    return edges[root].day;
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
