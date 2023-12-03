#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <vector>

// solution for Download Speed
// Dinic algo based on https://e-maxx.ru/algo/dinic
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
    edge e1 = { a, b, cap, 0 };
    edge e2 = { b, a, 0, 0 };
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
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  dinic g(n);
  for ( int i = 0; i < m; ++i )
  {
    int a, b, c;
    cin>>a>>b>>c;
    g.add_edge(a-1, b-1, c);
  }
  printf("%ld\n", g.calc());
printTime("end");
}