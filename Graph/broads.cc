#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stack>

// Building Roads

using namespace std;

struct node
{
  vector<int> edges;
  bool visited;
};

struct graph
{
  vector<node> nodes;
  vector<int> clusters;
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
  void dfs(int i)
  {
    stack<node *> st;
    st.push(&nodes[i]);
    clusters.push_back(1+i);
    nodes[i].visited = true;
    while( !st.empty() )
    {
      auto c = st.top(); st.pop();
      for ( int ci: c->edges )
      {
        if ( !nodes[ci].visited )
        {
          nodes[ci].visited = true;
          st.push(&nodes[ci]);
        }
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
    if ( !g.nodes[i].visited )
    {
#ifdef DEBUG
  printf("dfs %d cl %ld\n", i, g.clusters.size());
#endif
      g.dfs(i);
    }
  }
  auto cs = g.clusters.size();
  if ( !cs )
    puts("0");
  else {
    printf("%ld\n",  cs - 1);
    if ( cs != 1 )
      for ( int i = 1; i < cs; ++i )
        printf("%d %d\n", g.clusters[i-1], g.clusters[i]);
  }
}