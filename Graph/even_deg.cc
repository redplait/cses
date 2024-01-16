#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>

// solution for Even Outdegree Edges
// based on https://www.geeksforgeeks.org/convert-undirected-connected-graph-to-strongly-connected-directed-graph/
// 0.15s on test 6
using namespace std;

struct node
{
  int n;
  vector<int> edges;
  inline int degree() const
  {
    return edges.size();
  }
  int order = 0;
  int bridge_detect = 0;
};

struct graph
{
  int n, timer = 0;
  vector<node> nodes;
  vector<pair<int, int> > res;
  char *odd;
  graph(int N, int m)
   : n(N), nodes(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
    res.reserve(m);
    odd = (char *)calloc(N, 1);
  }
  inline void add_edge(int a, int b)
  {
    if ( a == b )
      return;
    node &n = nodes[a-1];
    n.edges.push_back(b-1);
    node &m = nodes[b-1];
    m.edges.push_back(a-1);
  }
  inline bool is_even(int v)
  {
    return !odd[v];
  }
  void dfs(int v, int p)
  {
#ifdef DEBUG
printf("dfs %d p %d\n", v, p);
#endif
    nodes[v].order = ++timer;
    for ( auto u: nodes[v].edges )
    {
      if ( p == u )
        continue;
      if ( !nodes[u].order )
      {
        dfs(u, v);
        if (is_even(u))
        {
          res.push_back({v, u});
          odd[v] ^= 1;
        } else {
          res.push_back({u, v});
          odd[u] ^= true;
        }
      } else {
        if ( nodes[u].order < nodes[v].order )
        {
          odd[v] ^= 1;
          res.push_back({v, u});
        }
      }
    }
/*
    if ( nodes[v].bridge_detect == nodes[v].order && p != -1 )
    {
      printf("IMPOSSIBLE");
      exit(0);
    } */
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  graph g(n, m);
  for ( int i = 0; i < m; ++i )
  {
    int a, b;
    cin>>a>>b;
    g.add_edge(a, b);
  }
  for ( int i = 0; i < n; ++i )
  {
    if ( !g.nodes[i].order ) g.dfs(i, -1);
  }
  if ( any_of(g.odd, g.odd + n, [](char c) { return c == 1; } ) )
    printf("IMPOSSIBLE");
  else
    for ( auto r: g.res )
      printf("%d %d\n", r.first + 1, r.second + 1);
}