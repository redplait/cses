#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <unordered_set>
#include <set>
#include <algorithm>

// Longest Flight Route

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
  int level = 0;
  unordered_set<int> in_edges;
  unordered_set<int> edges;
  inline size_t degree()
  {
    return edges.size();
  }
};

struct graph
{
  vector<node> nodes;
  vector<int> parent;
  char *visited;
  graph(int N)
   : nodes(N), parent(N)
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
  int max_in(int t)
  {
    int l = -1;
    int s = 0;
    for ( auto e: nodes[t].in_edges )
    {
      if ( nodes[e].level > l )
      { l = nodes[e].level; s = e; }
    }
    return s;
  }
  void dfs(int s, int level)
  {
    visited[s] = 1;
    nodes[s].level = level;
    for ( auto e: nodes[s].edges )
    {
      if ( !visited[e] )
      {
        parent[e] = s;
        dfs(e, level+1);
      } else if ( nodes[e].level < nodes[s].level + 1 )
      {
        nodes[e].level = nodes[s].level + 1;
        parent[e] = s;
      }
    }
  }
  void dfs2()
  {
    set<int> q;
    for ( auto &e: nodes )
    {
      if ( !visited[e.n] ) continue;
      if ( !e.degree() ) continue;
      for ( int ci: e.edges )
      {
        if ( e.level + 1 > nodes[ci].level )
        {
          nodes[ci].level =  e.level + 1;
          parent[ci] = e.n;
          q.insert(ci);
        }
      }
    }
    while( !q.empty() )
    {
      int v = *q.begin();
      q.erase(q.begin());
      for ( int ci: nodes[v].edges )
      {
        if ( nodes[v].level + 1 > nodes[ci].level )
        {
          nodes[ci].level = nodes[v].level + 1;
          parent[ci] = nodes[v].n;
          q.insert(ci);
        }
      }
    }
  }
  void dump()
  {
    for ( auto &n: nodes )
    {
      if ( !visited[n.n] ) continue;
      printf("%d l %d parent %d\n", n.n + 1, n.level, parent[n.n] + 1);
    }
  }
  inline void add_edge(int a, int b)
  {
    if ( a == b )
      return;
    node &n = nodes[a-1];
    n.edges.insert(b-1);
    node &m = nodes[b-1];
    m.in_edges.insert(a-1);
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
  g.dfs(0, 0);
  if ( !g.visited[n-1] )
  {
    puts("IMPOSSIBLE");
    return 0;
  }
  g.dfs2();
#ifdef DEBUG
  g.dump();
#endif
  // build path from n-1 till 0
  vector<int> path;
  for ( int i = n - 1; i; i = g.parent[i] )
    path.push_back(i);
  // last zero node
  path.push_back(0);
  reverse(path.begin(), path.end());
  printf("%ld\n", path.size());
  for ( int e: path )
    printf("%d ", e + 1);
}