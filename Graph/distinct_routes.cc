#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <vector>
#include <list>

// solution for Distinct Routes
// Dinic algo based on https://e-maxx.ru/algo/dinic
// after calculation of maxflow we also need to get all paths - set of edges with non-zero flow AND even edges index
// bcs odd edges indexes are for back direction
// we can use each edge only 1 time so mark each used in some path edges - this is field used in struct edge
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

struct edge {
 int a, b, cap, flow;
 char used;
};

struct dinic
{
  int N;
  int curr_edge = 0;
  vector<int> d, ptr, q;
  vector<edge> edges;
  vector<vector<int> > g;
  dinic(int n): N(n), d(n), ptr(n), q(n), g(n)
  { }
  void add_edge (int a, int b, int cap)
  {
    edge e1{ a, b, cap, 0, 0 };
    edge e2{ b, a, 0, 0, 0 };
    g[a].push_back( curr_edge++ );
    edges.push_back(e1);
    g[b].push_back( curr_edge++ );
    edges.push_back(e2);
  }
  bool bfs()
  {
    int qh=0, qt=0;
    q[qt++] = 0;
    fill(d.begin(), d.end(), -1);
    d[0] = 0;
    while (qh < qt && d[N-1] == -1)
    {
	int v = q[qh++];
	for (size_t i=0; i<g[v].size(); ++i) {
	    int id = g[v][i],
		to = edges[id].b;
	    if (d[to] == -1 && edges[id].flow < edges[id].cap) {
		q[qt++] = to;
		d[to] = d[v] + 1;
	    }
	}
    }
    return d[N-1] != -1;
  }
  int dfs (int v, int flow)
  {
    if (!flow) return 0;
    if (v == N-1) return flow;
    for (; ptr[v]<(int)g[v].size(); ++ptr[v]) {
	int id = g[v][ptr[v]],
	    to = edges[id].b;
	if (d[to] != d[v] + 1)  continue;
	int pushed = dfs (to, min (flow, edges[id].cap - edges[id].flow));
	if (pushed) {
	    edges[id].flow += pushed;
	    edges[id^1].flow -= pushed;
	    return pushed;
	}
    }
    return 0;
  }
  int64_t calc()
  {
    int64_t flow = 0;
    for (;;) {
	if (!bfs()) break;
        fill(ptr.begin(), ptr.end(), 0);
	while (int pushed = dfs (0, INT_MAX))
	  flow += pushed;
    }
    return flow;
  }
  bool find_route(int from, list<int> &path)
  {
    path.push_back(from);
    if ( from == N - 1 ) return true;
    for ( int i: g[from] )
    {
      if ( i & 1 || edges[i].used ) continue;
      if ( edges[i].flow )
      {
        edges[i].used = 1;
        return find_route(edges[i].b, path);
      }
    }
    return false;
  }
  void get_routes(vector<list<int> > &res)
  {
    list<int> tmp;
    while(find_route(0, tmp) ) res.push_back( move(tmp) );
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  dinic g(n);
  for ( int i = 0; i < m; ++i )
  {
    int a, b;
    cin>>a>>b;
    g.add_edge(a-1, b-1, 1);
  }
  printf("%ld\n", g.calc());
printTime("calc");
  vector<list<int> > routes;
  g.get_routes(routes);
  for (auto r : routes)
  {
    printf("%ld\n", r.size());
    for (auto i : r) printf("%d ", 1+i);
    printf("\n");
  }
printTime("end");
}