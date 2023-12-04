#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm> // for sort

// Course Schedule
// nothing special - first check for loops in graph and then make topological sorting
// output order is just reverse of topo sort
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

struct sched
{
  int N;
  vector<char> visited, in_stack;
  vector<int> top;
  vector<vector<int> > g;
  sched(int n): N(n), visited(n), in_stack(n), g(n)
  { }
  void read(int q)
  {
    while(q--)
    {
      int a, b;
      cin>>a>>b;
      a--; b--;
      g[a].push_back(b);
    }
  }
  bool dfs(int s)
  {
    visited[s] = 1;
    in_stack[s] = 1;
    bool res = false;
    for (auto i : g[s])
    {
      if (in_stack[i]) return true;
      if (visited[i]) continue;
      res |= dfs(i);
    }
    in_stack[s] = 0;
    return res;
  }
  void dfs_sort(int s)
  {
    visited[s] = 1;
    for ( int i: g[s] )
      if ( !visited[i] ) dfs_sort(i);
    // put vertex in top after exit of dfs
    top.push_back(s);
  }
  bool scan_loops()
  {
    bool res = false;
    for ( int i = 0; i < N; ++i )
      if ( !visited[i] )
      {
        res |= dfs(i);
        if ( res ) return res;
      }
    return res;
  }
  void topo_sort()
  {
    fill(visited.begin(), visited.end(), 0);
    for ( int i = 0; i < N; ++i )
      if ( !visited[i]) dfs_sort(i);
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  sched g(n);
  g.read(m);
  if ( g.scan_loops() )
  {
    puts("IMPOSSIBLE");
    return 0;
  }
  g.topo_sort();
  reverse(g.top.begin(), g.top.end());
  for ( int i: g.top ) printf("%d ", 1+i);
  printf("\n");
}