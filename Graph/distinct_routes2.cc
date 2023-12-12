#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>
#include <limits.h>

// solution for Distinct Routes II
// very similar to Grid Puzzle II (and actually I ripped almost all code from it)
// we again asked to find min-cost max-flow in graph, like https://algorithmica.org/ru/mincost-maxflow
// all costs are 1
// but we need to find flow for K days only
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")
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

struct Edge {
  int u, v, cap, cost;
  char used = 0;
};

struct sq
{
  vector<int> parents;
  vector<Edge *> edges, redges;
  vector<int64_t> dist;
  vector<char> visited;
  vector<vector<int> > G;
  int edge_idx, N, t_idx;
  int64_t res_v;
  sq(int n):
    parents(n), dist(n), visited(n), G(n),
    edge_idx(1), N(n), t_idx(N-1), res_v(0)
  {
    edges.push_back(nullptr); redges.push_back(nullptr);
  }
  ~sq()
  {
    for ( auto e: edges ) if ( e ) delete e;
    for ( auto e: redges ) if ( e ) delete e;
  }
  void read(int m)
  {
    for ( int i = 0; i < m; i++ )
    {
      int u, v;
      cin>>u>>v;
      u--; v--;
      G[u].push_back(edge_idx);
      G[v].push_back(-edge_idx);
      Edge *dir = new Edge{ u, v, 1, 1 };
      edges.push_back(dir);
      Edge *rev = new Edge{ v, u, 0, -1 };
      redges.push_back(rev);
      edge_idx++;
    }
  }
  void dfs(vector<int> &path, int v)
  {
    path.push_back(v);
#ifdef DEBUG
printf("dfs: %d\n", v);
#endif
    if ( t_idx == v ) return;
    for ( auto n: G[v] )
    {
#ifdef DEBUG
if ( n > 0 ) {
  printf("%d: %d", v, n);
  if ( edges[n] ) printf(" %d cap %d cost %d", edges[n]->v, edges[n]->cap, edges[n]->cost);
  printf("\n"); }
#endif
      if (n > 0 && !edges[n]->cap && !edges[n]->used )
      {
        edges[n]->used = 1;
        dfs(path, edges[n]->v);
        return;
      }
    }
#ifdef DEBUG
printf("end dfs %d\n", v);
#endif
  }
  void dump(int k)
  {
    printf("%ld\n", res_v);
    for( int i = 0; i < k; i++)
    {
      vector<int> path;
      dfs(path, 0);
      printf("%ld\n", path.size());
      for ( int v: path ) printf("%d ", 1+v);
      printf("\n");
    }
  }
  void bellman_ford()
  {
    fill(visited.begin(), visited.end(), 0);
    fill(dist.begin(), dist.end(), LONG_MAX);
    fill(parents.begin(), parents.end(), -1);

    queue<int> Q;
    Q.push(0);
    dist[0] = 0;
    visited[0] = 1;
    while(!Q.empty())
    {
      int u = Q.front(); Q.pop();
      visited[u] = false;

      for(int i : G[u])
      {
        Edge *e = (i < 0 ? redges[-i] : edges[i]);
        if(e->cap > 0 && dist[e->v] > dist[u] + e->cost)
        {
          dist[e->v] = dist[u] + e->cost;
          parents[e->v] = i;
          if(!visited[e->v])
          {
            visited[e->v] = 1;
            Q.push(e->v);
          }
        }
      }
    }
  }
  inline void mark_rev(Edge *er)
  {
    // check for Source or Target
    if ( !er->v ) return;
    if ( er->u == t_idx ) return;
  }
  inline void mark_dir(Edge *ed)
  {
    if ( !ed->u ) return;
    if ( ed->v == t_idx ) return;
  }
  int calc(int k)
  {
    int flow = 0;
    while(flow < k)
    {
        bellman_ford();
        if(dist[t_idx] == LONG_MAX) break;

        int aug = k-flow;
        int u = t_idx;
        while(u)
        {
            Edge *e = (parents[u] < 0 ? redges[-parents[u]] : edges[parents[u]]);
            aug = min(aug, e->cap);
            u = e->u;
        }

        flow += aug;
        res_v += aug * dist[t_idx];
        u = t_idx;
        while(u)
        {
            if (parents[u] < 0)
            {
              Edge *er = redges[-parents[u]],
                   *ed = edges[-parents[u]];
              er->cap -= aug;
              ed->cap += aug;
              u = er->u;
              // if ( !er->cap ) mark_rev(er);
              // if ( !ed->cap ) mark_dir(ed);
            } else {
              Edge *er = redges[parents[u]],
                   *ed = edges[parents[u]];
              er->cap += aug;
              ed->cap -= aug;
              u = ed->u;
              // if ( !er->cap ) mark_rev(er);
              // if ( !ed->cap ) mark_dir(ed);
            }
        }
    }
    return (flow < k ? 0 : 1);
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m, k;
  cin>>n>>m>>k;
  sq g(n);
  g.read(m);
printTime("read");
  if ( !g.calc(k) ) puts("-1");
  else g.dump(k);
printTime("calc");
}