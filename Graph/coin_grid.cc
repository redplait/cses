#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <queue>

// solution for Coin Grid
// rows and columns are actually form bipartite graph
// so we can use Hopcroft-Carp algo (stealed from https://judge.yosupo.jp/submission/52112)
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

struct sq
{
  int N, n_left, n_right, flow = 0;
  vector<vector<int> > g;
  vector<int> match_from_left, match_from_right;
  vector<int> dist;
  list<pair<char, int> > res;
  sq(int n)
   : N(n), n_left(n), n_right(n), g(n), match_from_left(n, -1), match_from_right(n, -1), dist(n)
  {}
  // read strings from cin and fill bipartite graph g indexed by rows numbers
  void prepare()
  {
    for ( int i = 0; i < N; i++ )
    {
      string s;
      cin>>s;
      for ( int j = 0; j < N; ++j )
      {
        if ( s.at(j) == 'o' )
          g[i].push_back(j);
      }
    }
  }
  void bfs() 
  {
    queue<int> q;
    for (int u = 0; u < n_left; ++u) {
        if (!~match_from_left[u])
        {
          q.push(u);
          dist[u] = 0;
        } else
          dist[u] = -1;
    }
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (auto v : g[u])
          if (~match_from_right[v] && !~dist[match_from_right[v]]) 
          {
              dist[match_from_right[v]] = dist[u] + 1;
              q.push(match_from_right[v]);
          }
    }
  }
  bool dfs(int u) 
  {
    for (auto v : g[u])
      if (!~match_from_right[v]) {
          match_from_left[u] = v, match_from_right[v] = u;
          return true;
      }
    for (auto v : g[u])
      if (dist[match_from_right[v]] == dist[u] + 1 && dfs(match_from_right[v])) {
            match_from_left[u] = v, match_from_right[v] = u;
            return true;
      }
    return false;
  }
  void minimum_vertex_cover() 
  {
    for (int u = 0; u < n_left; ++u) {
      if (!~dist[u])
        res.push_back({1, 1+u});
      else if (~match_from_left[u])
        res.push_back({ 2, 1+match_from_left[u]});
    }
  }
  int calc()
  {
    while (true) {
      bfs();
      int augment = 0;
      for (int u = 0; u < n_left; ++u)
          if (!~match_from_left[u]) augment += dfs(u);
      if (!augment) break;
      flow += augment;
    }
    minimum_vertex_cover();
    return flow;
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n;
  cin>>n;
  sq g(n);
  g.prepare();
printTime("prepare");
  printf("%d\n", g.calc());
printTime("calc");
  for ( auto &w: g.res ) printf("%d %d\n", w.first, w.second);
}