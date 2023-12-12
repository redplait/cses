#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <stack>

// Planets and Kingdoms
// directed graph and we need connected components of it
// so use Kosaraju algo - like https://e-maxx.ru/algo/strong_connected_components

using namespace std;

inline float getTime()
{
  return (float)clock()/CLOCKS_PER_SEC;
}

inline void printTime(const char *pfx)
{
#ifdef TIME
  printf("%s: %f\n", pfx, getTime());
#endif
}

struct node
{
  int n;
  vector<int> in_edges;
  vector<int> edges;
  int id = -1;
};

struct graph
{
  int N;
  vector<node> nodes;
  char *visited;
  stack<int> st;
  int sc_index = 0;
  graph(int n) : N(n), nodes(n)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
    visited = (char *)malloc(N);
    reset();
  }
  void reset()
  {
    memset(visited, 0, nodes.size());
  }
  inline void add_edge(int a, int b)
  {
    if ( a == b )
      return;
    node &n = nodes[a-1];
    n.edges.push_back(b-1);
    node &m = nodes[b-1];
    m.in_edges.push_back(a-1);
  }
  // via out-edges, fill stack
  void dfs(int v)
  {
    visited[v] = 1;
    for ( int u: nodes[v].edges )
    {
      if ( visited[u] ) continue;
      dfs(u);
    }
    st.push(v);
  }
  // via in-edges
  void dfs_in(int v)
  {
    visited[v] = 1;
    nodes[v].id = sc_index;
    for ( int u: nodes[v].in_edges )
    {
      if ( visited[u] ) continue;
      dfs_in(u);
    }
  }
  void calc()
  {
    for ( int i = 0; i < N; ++i )
    {
      if ( !visited[i] ) dfs(i);
    }
    reset();
    while ( !st.empty() )
    {
      int v = st.top(); st.pop();
      if ( visited[v] ) continue;
      dfs_in(v);
      sc_index++; 
    }
  }
  void dump() const
  {
    printf("%d\n", sc_index);
    for ( int i = 0; i < N; i++ )
     printf("%d ", nodes[i].id + 1);
    printf("\n");
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
printTime("read");
  g.calc();
printTime("calc");
  g.dump();  
}