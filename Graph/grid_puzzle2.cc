#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <numeric>
#include <limits.h>

// solution for Grid Puzzle II
// pretty obviously that we asked to find max-flow in graph, like https://algorithmica.org/ru/mincost-maxflow
// main question is how to build such graph
// Lets add N vertices for columns and N for rows
// then yet 2 pseudo vertices S for source (traditionally has index 0) and T to get results
// now we can link S with each row (see method add_row) and each column with T (see method add_col)
// For squads we can link it`s row and column over this squad
// so total amount of vertices is 2 * N + 2
// Another variant also appropriate - we could link S with columns, rows with T and for each squad link column with row
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

// u - row, starting from 1 (bcs 0 is Source), v - column
struct Edge {
  int u, v, cap, val;
};

struct sq
{
  vector<string> res;
  vector<int> rows, cols, parents;
  vector<Edge *> edges, redges;
  vector<int64_t> dist;
  vector<char> visited;
  vector<set<int> > G;
  int edge_idx, N, t_idx, rv;
  int64_t res_v;
  sq(int n): res(n), rows(n), cols(n),
    parents(2 * (n + 1)), dist(2 * (n + 1)), visited(2 * (n + 1)), G(2 * (n + 1)),
    edge_idx(1), N(n), res_v(0)
  {
    for ( int i = 0; i < N; ++i ) res[i].resize(N, '.');
    t_idx = 1 + 2 * N;
    edges.push_back(nullptr); redges.push_back(nullptr);
  }
  ~sq()
  {
    for ( auto e: edges ) if ( e ) delete e;
    for ( auto e: redges ) if ( e ) delete e;
  }
  inline void add_row(int r, int v)
  {
    rows[r] = v;
    if ( v ) {
      G[0].insert(edge_idx);
      G[r+1].insert(-edge_idx);
      Edge *dir = new Edge{ 0, r+1, v, 0};
      edges.push_back(dir);
      Edge *rev = new Edge{ dir->v, dir->u, 0, 0};
      redges.push_back(rev);
      edge_idx++;
    }
  }
  inline void add_col(int c, int v)
  {
    cols[c] = v;
    if ( v ) {
      G[c+1+N].insert(edge_idx);
      G[t_idx].insert(-edge_idx);
      Edge *dir = new Edge{c+1+N, t_idx, v, 0};
      edges.push_back(dir);
      Edge *rev = new Edge{dir->v, dir->u, 0, 0};
      redges.push_back(rev);
      edge_idx++;
    }
  }
  int quick_check()
  {
    rv = accumulate(rows.begin(), rows.end(), 0);
    auto cv = accumulate(cols.begin(), cols.end(), 0);
#ifdef DEBUG
 printf("rv %ld vc %ld\n", rv, cv);
#endif
    return rv == cv ? 1 : 0;
  }
  void add_item(int r, int c, int v)
  {
    if (!rows[r] || !cols[c] ) return;
    G[1+r].insert(edge_idx);
    G[c+1+N].insert(-edge_idx);
    Edge *dir = new Edge{1+r, c+1+N, 1, -v};
    edges.push_back(dir);
    Edge *rev = new Edge{dir->v, dir->u, 0, v};
    redges.push_back(rev);
    edge_idx++;
  }
  void dump()
  {
    printf("%ld\n", -res_v);
    for ( size_t i = 1; i < edges.size(); ++i ) if ( !edges[i]->cap ) mark_dir(edges[i]);
    for ( auto &s: res ) printf("%s\n", s.c_str());
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
        if(e->cap > 0 && dist[e->v] > dist[u] + e->val)
        {
          dist[e->v] = dist[u] + e->val;
          parents[e->v] = i;
          if(!visited[e->v])
          {
            visited[e->v] = true;
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
    res[er->v - 1].at(er->u - 1 - N) = 'X';
  }
  inline void mark_dir(Edge *ed)
  {
    if ( !ed->u ) return;
    if ( ed->v == t_idx ) return;
    res[ed->u - 1].at(ed->v - 1 - N) = 'X';
  }
  int calc()
  {
    int flow = 0;
    while(flow < rv)
    {
        bellman_ford();
        if(dist[t_idx] == LONG_MAX) break;

        int aug = rv-flow;
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
    return (flow < rv ? 0 : 1);
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n;
  cin>>n;
  sq g(n);
  // read rows
  for ( int i = 0; i < n; ++i )
  {
    int v;
    cin>>v;
    g.add_row(i, v);
  }
  // read columns
  for ( int i = 0; i < n; ++i )
  {
    int v;
    cin>>v;
    g.add_col(i, v);
  }
  if ( !g.quick_check() ) puts("-1");
  else {
    for ( int r = 0; r < n; r++ )
      for ( int c = 0; c < n; c++ )
      {
        int v;
        cin>>v;
        g.add_item(r, c, v);
      }
printTime("read");
    if ( !g.calc() ) puts("-1");
    else g.dump();
printTime("calc");
  }
}