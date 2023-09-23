#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stack>

// Building Teams
// actially we need to build bipartite graph

using namespace std;

struct node
{
  vector<int> edges;
  char g; // group number
};

struct graph
{
  vector<node> nodes;
  graph(int N)
   : nodes(N)
  {}
  inline void add_edge(int a, int b)
  {
    node &n = nodes[a-1];
    n.edges.push_back(b-1);
    node &m = nodes[b-1];
    m.edges.push_back(a-1);
  }
  void color(int u)
  {
    for (int i: nodes[u].edges)
    {
      node &v = nodes[i];
      if ( !v.g )
      {
        v.g = 3 - nodes[u].g;
        color(i);
      } else if ( v.g == nodes[u].g )
      {
        printf("IMPOSSIBLE");
        exit(0);
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
    if ( !g.nodes[i].g )
    {
#ifdef DEBUG
  printf("color %d\n", i);
#endif
      g.nodes[i].g = 1;
      g.color(i);
    }
  }
  for ( int i = 0; i < n; ++i )
   printf("%d ", g.nodes[i].g);
}