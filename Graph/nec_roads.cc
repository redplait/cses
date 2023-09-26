#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <set>

// Necessary Roads

using namespace std;

struct node
{
  int n;
  vector<int> edges;
  inline int degree() const
  {
    return edges.size();
  }
  int tin, fup;
  char visited = 0;
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
  inline void add_edge(int a, int b)
  {
    node &n = nodes[a-1];
    n.edges.push_back(b-1);
    node &m = nodes[b-1];
    m.edges.push_back(a-1);
  }
  int timer = 0;
  // result. order of nodes first < second
  set<pair<int, int> > res;
  inline void insert_res(int a, int b)
  {
    if ( a > b )
     res.insert({b, a});
    else
     res.insert({a, b});
  }
  // based on https://e-maxx.ru/algo/bridge_searching
  // complexity O(V + E)
  void dfs(int v, int p = -1)
  {
    nodes[v].visited = 1;
    nodes[v].tin = nodes[v].fup = timer++;
    for ( int to: nodes[v].edges )
    {
      if (to == p) // cycle to itself
        continue;
      if (nodes[to].visited)
        nodes[v].fup = min(nodes[v].fup, nodes[to].tin);
      else
      {
        dfs (to, v);
        nodes[v].fup = min(nodes[v].fup, nodes[to].fup);
        if (nodes[to].fup > nodes[v].tin )
          insert_res(v, to);
      }
    }
  }
};

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
  for ( int i = 0; i < n; ++i )
  {
    if ( !g.nodes[i].visited ) // graph can have several disjointed parts
     g.dfs(i);
  }
  printf("%ld\n", g.res.size());
  for ( auto &r: g.res )
    printf("%d %d\n", r.first + 1, r.second + 1);
}