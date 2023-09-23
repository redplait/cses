#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>

// Message Route

using namespace std;

struct node
{
  int n;
  vector<int> edges;
  bool visited;
  int from; // for backtracking
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
  void dfs(int i, int t)
  {
    // run wave from node i
    queue<node *> q;
    q.push(&nodes[i]);
    nodes[i].visited = true;
    while( !q.empty() )
    {
      auto c = q.front(); q.pop();
      for ( int ci: c->edges )
      {
        if ( nodes[ci].visited )
          continue;
#ifdef DEBUG
 printf("add %d from %d\n", ci, c->n);
#endif
        nodes[ci].from = c->n;
        nodes[ci].visited = true;
        if ( ci == t )
          return;
        q.push(&nodes[ci]);
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
  g.dfs(0, n - 1);
  if ( !g.nodes[n-1].visited )
    printf("IMPOSSIBLE");
  else {
    vector<int> path;
    auto *curr = &g.nodes[n-1];
    while(curr->n)
    {
      path.push_back(curr->n);
      curr = &g.nodes[curr->from];
    }
    path.push_back(0);
    printf("%ld\n", path.size());
    for ( auto j = path.rbegin(); j != path.rend(); ++j )
      printf("%d ", 1 + *j);
  }
}