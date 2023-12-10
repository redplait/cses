#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>
#include <limits.h>

// solution for Task Assignment
// very similar to Grid Puzzle II (and actually I ripped almost all code from it)
// we again asked to find min-cost max-flow in graph, like https://algorithmica.org/ru/mincost-maxflow
// graph can be build as S with index 0
// then edges from S to each employee at 1 .. N with capacity 1
// from each employee edge to it`s cost at 1 + N .. 2 * N
// and finally edge from costs to T with index 2 * N + 1
// total amount of vertices is 2 * N + 2 = 2 * (N + 1)
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

// u - emplpoyee, starting from 1 (bcs 0 is Source)
struct Edge {
  int u, v, cap, cost;
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
    parents(2 * (n + 1)), dist(2 * (n + 1)), visited(2 * (n + 1)), G(2 * (n + 1)),
    edge_idx(1), N(n), res_v(0)
  {
    t_idx = 1 + 2 * N;
    edges.push_back(nullptr); redges.push_back(nullptr);
  }
  ~sq()
  {
    for ( auto e: edges ) if ( e ) delete e;
    for ( auto e: redges ) if ( e ) delete e;
  }
  inline void add_ec(int u, int v, int cost)
  {
      G[u].push_back(edge_idx);
      G[v].push_back(-edge_idx);
      Edge *dir = new Edge{ u, v, 1, cost};
      edges.push_back(dir);
      Edge *rev = new Edge{ v, u, 0, -cost};
      redges.push_back(rev);
      edge_idx++;
  }
  void dfs(vector<int> &path, int v)
  {
#ifdef DEBUG
printf("dfs: %d\n", v);
#endif
    if ( t_idx == v ) return;
    if ( v ) path.push_back(v);
    for ( auto n: G[v] )
    {
#ifdef DEBUG
if ( n > 0 ) {
  printf("%d: %d", v, n);
  if ( edges[n] ) printf(" %d cap %d cost %d", edges[n]->v, edges[n]->cap, edges[n]->cost);
  printf("\n"); }
#endif
      if (n > 0 && !edges[n]->cap )
      {
        dfs(path, edges[n]->v);
        return;
      }
    }
#ifdef DEBUG
printf("end dfs %d\n", v);
#endif
  }
  void dump()
  {
    printf("%ld\n", res_v);
    for( int i = 1; i <= N; i++)
    {
      vector<int> path;
      dfs(path, i);
      printf("%d %d\n", path[0], path[1]-N);
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
  int calc()
  {
    // link S with each vertex 1 .. N
    for (int v = 1; v <= N; v++)
    {
      G[0].push_back(edge_idx);
      G[v].push_back(-edge_idx);
      Edge *dir = new Edge{ 0, v, 1, 0};
      edges.push_back(dir);
      Edge *rev = new Edge{ v, 0, 0, 0};
      redges.push_back(rev);
      edge_idx++;
    }
    // link each vertex N + 1 .. 2 * N with T (index t_idx)
    for (int v = N+1; v <= 2*N; v++)
    {
      G[v].push_back(edge_idx);
      G[t_idx].push_back(-edge_idx);
      Edge *dir = new Edge{ v, t_idx, 1, 0};
      edges.push_back(dir);
      Edge *rev = new Edge{ t_idx, v, 0, 0};
      redges.push_back(rev);
      edge_idx++;
    }
    int flow = 0;
    while(flow < N)
    {
        bellman_ford();
        if(dist[t_idx] == LONG_MAX) break;

        int aug = N-flow;
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
    return (flow < N ? 0 : 1);
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n;
  cin>>n;
  sq g(n);
  for(int i = 1; i <= n; i++)
  {
    for(int j = n+1; j <= 2*n; j++)
    {
      int cost;
      cin>>cost;
      g.add_ec(i, j, cost);
    }
  }
printTime("read");
  if ( !g.calc() ) puts("-1");
  else g.dump();
printTime("calc");
}