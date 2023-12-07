#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <vector>
#include <queue>

// solution for Police Chase
// based on https://www.geeksforgeeks.org/minimum-cut-in-a-directed-graph/
// istead of having residual graph we can keep capacities in the same graph for each edge

using namespace std;
// first - vertex, second - capacity
typedef pair<int, int> edge;

struct graph
{
  vector<vector<edge> > nodes;
  vector<char> visited;
  vector<int> parents;
  int n;
  graph(int N)
   : nodes(N), visited(N), parents(N, -1), n(N)
  {}
  inline void add_edge(int a, int b)
  {
    auto &n = nodes[a-1];
    n.push_back({b-1, 1});
    auto &m = nodes[b-1];
    m.push_back({a-1, 1});
  }
  int bfs(int s, int t)
  {
    fill(visited.begin(), visited.end(), 0);
    queue <int> q;
    q.push(s);
    visited[s] = 1;
    parents[s] = -1;
    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        for (auto &v: nodes[u])
        {
            if (!visited[v.first] && v.second > 0)
            {
                q.push(v.first);
                parents[v.first] = u;
                visited[v.first] = 1;
            }
        }
    }
    // If we reached sink in BFS starting from source, then return
    // true, else false
    return visited[t];
  }
  void dfs(int s)
  {
    visited[s] = 1;
    for ( auto &e: nodes[s] )
      if ( !visited[e.first] && e.second )
        dfs(e.first);
  }
  inline int get_cap(int u, int v)
  {
    for ( auto &e: nodes[u] )
     if ( e.first == v ) return e.second;
    return -1;
  }
  inline void set_cap(int u, int v, int cap)
  {
    for ( auto &e: nodes[u] )
     if ( e.first == v ) { e.second += cap; break; }
  }
  void minCut(int s, int t)
  {
    while (bfs(s, t))
    {
        // Find minimum residual capacity of the edges along the
        // path filled by BFS. Or we can say find the maximum flow
        // through the path found.
        int path_flow = INT_MAX;
        for (int v=t; v!=s; v=parents[v])
        {
            int u = parents[v];
            path_flow = min(path_flow, get_cap(u, v));
        }
        // update residual capacities of the edges and reverse edges
        // along the path
        for (int v=t; v != s; v=parents[v])
        {
            int u = parents[v];
            set_cap(u, v, -path_flow);
            set_cap(v, u, path_flow);
        }
    }
    fill(visited.begin(), visited.end(), 0);
    dfs(s);
    vector<edge> res;
    for (int i = 0; i < n; i++)
    {
      for ( auto &e: nodes[i] )
        if ( visited[i] && !visited[e.first] )
          res.push_back({i, e.first});
    }
    printf("%ld\n", res.size());
    for ( auto &r: res ) printf("%d %d\n", 1+r.first, 1+r.second);
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
  g.minCut(0, n-1);
}