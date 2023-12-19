#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <limits.h>
#include <stack>
#include <set>

// solution for Coin Collector
// directed graph so lets condensate it with Kosaraju algo 
// like https://e-maxx.ru/algo/strong_connected_components
// Next we could use Floyd–Warshall algorithm with weight of each edge to node A eq amount of coins in A
// however it`s complexity is O(N ^ 3)
// So let`s run waves for each node in condensed graph and find path with max coins in visited nodes
// Also we can cache already visited nodes
// complexity of this waves is O(N * N), with caching just O(N)

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
  int value;
  vector<int> in_edges;
  vector<int> edges;
  int id = -1;
};

struct cond_node
{
  set<int> edges;
  int64_t value = 0;
  int64_t cvalue = 0;
};

struct graph
{
  int N;
  vector<node> nodes;
  vector<cond_node> cond;
  char *visited;
  stack<int> st;
  int sc_index = 0;
  graph(int n) : N(n), nodes(n)
  {
    for ( int i = 0; i < N; ++i )
    {
      nodes[i].n = i;
      cin>>nodes[i].value;
    }
    visited = (char *)malloc(N);
    reset();
  }
  void reset()
  {
    memset(visited, 0, nodes.size());
  }
  void reset_cond()
  {
    memset(visited, 0, cond.size());
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
  int64_t wave(int i)
  {
    visited[i] = 1;
    int64_t res = 0;
    for ( int e: cond[i].edges )
    {
      if ( !visited[e] ) wave(e);
#ifdef DEBUG
 printf("wave from %d val %ld to %d res %ld\n", i, cond[i].value, e, res);
#endif
      res = max(res, cond[e].cvalue);
    }
    cond[i].cvalue = res + cond[i].value;
    return cond[i].cvalue;
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
#ifdef DEBUG
    printf("scc %d\n", sc_index);
#endif
    // lets make condensed graph
    cond.resize(sc_index);
    for ( int i = 0; i < N; ++i )
    {
      int scc = nodes[i].id;
      cond[scc].value += nodes[i].value;
#ifdef DEBUG
 printf("node %d value %ld in SCC %d\n", i, nodes[i].value, scc);
#endif
      for ( int e: nodes[i].edges )
        if ( nodes[e].id != scc )
          cond[scc].edges.insert(nodes[e].id);
    }
printTime("condensed");
    // now run waves
    reset_cond();
    int64_t res = LONG_MIN;
    for ( int i = 0; i < sc_index; i++ )
    {
      if ( visited[i] ) continue;
      res = max(res, wave(i));
#ifdef DEBUG
 printf("> %d %ld\n", i, res);
#endif
    }
    // dump result
    printf("%ld\n", res);
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
}