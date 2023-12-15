#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <limits.h>

// solution for High Score
// classical Bellman-Ford with negative cycle detection, for example from
// https://e-maxx.ru/algo/ford_bellman
// Bcs we try to find max cost we can just negate all weights
// Also we need to find not ANY negative cycle but only reacheable from T

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

const int64_t INF = 10000000000000;

struct edge
{
  int a, b, cost;
};

struct bf
{
  int N;
  vector<edge> e;
  vector<int64_t> dist;
  vector<char> reach;
  vector< vector<int> > adj_rev;
  bf(int n): N(n), dist(n, INF), reach(N), adj_rev(N)
  { e.reserve(n); }
  void read(int m)
  {
    for ( int i = 0; i < m; i++ )
    {
      edge curr;
      cin>>curr.a>>curr.b>>curr.cost;
      curr.a--; curr.b--;
      adj_rev[curr.b].push_back(curr.a);
      curr.cost = -curr.cost;
      e.push_back(curr);
    }
  }
  void dfs_rev(int v)
  {
    reach[v] = 1;
    for ( auto n: adj_rev[v] ) if ( !reach[n] ) dfs_rev(n);
  }
  int64_t calc(int m)
  {
    dfs_rev(N-1);
    dist[0] = 0;
    int x = -1;
    for (int i=0; i<N; ++i)
    {
	x = -1;
	for (int j=0; j<m; ++j)
	    if (dist[e[j].a] < INF)
		if (dist[e[j].b] > dist[e[j].a] + e[j].cost) {
		    dist[e[j].b] = max (-INF, dist[e[j].a] + e[j].cost);
		    if ( reach[ e[j].b ] ) x = e[j].b;
		}
    }
    if ( -1 != x ) { 
#ifdef DEBUG
     printf("x %d\n", x);
#endif
     puts("-1"); exit(0); }
    return -dist[N-1];
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  bf g(n);
  g.read(m);
printTime("read");
  printf("%ld\n", g.calc(m));
printTime("res");
}