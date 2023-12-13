#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <limits.h>
#include <string.h>
#include <set>
#include <algorithm>

// solution for Flight Discount
// I could use Yen`s algo to get all K-th shortest paths - complexity is O(K * N * (M + N * log N))
// there are 2 problems
// 1) how big is K?
// 2) whithin each found path we also need to find edge with maximal cost for discount so total complexity is even worse N ^ 3 * log(N)
// So lets try more simple way - we could mark nodes reacheable from T
// then run Dijkstra twice for all such nodes:
// 1) to calculate distance from 0 (S) to all vertices
// 2) to calculate distance from T to all vertices using in-edges
// and then just enum all edges a-b with cost C like dist_S[a] + dist_T[b] + C / 2

using namespace std;

struct node
{
  int n;
  vector<pair<int, int> > edges;
  vector<pair<int, int> > in_edges;
  bool reach = false;
  inline int degree() const
  {
    return (int)edges.size();
  }
  inline int in_degree() const
  {
    return (int)in_edges.size();
  }
};

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

struct graph
{
  vector<node> nodes;
  vector<int64_t> dist, dist_rev;
  // vector<node *> parents;
  int edge_count = 0;
  int N;
//  char *visited;
  graph(int n)
   : nodes(n), dist(n, LONG_MAX), dist_rev(n, LONG_MAX), N(n)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
//    visited = (char *)malloc(N);
//    reset();
  }
  void reset()
  {
//    memset(visited, 0, nodes.size());
  }
  inline void add_edge(int a, int b, int k)
  {
    if ( a == b )
      return;
    node &n = nodes[a-1];
    n.edges.push_back( { b-1, k } );
    edge_count++;
    node &m = nodes[b-1];
    m.in_edges.push_back({ a-1, k} );
  }
  // mark reacheble nodes
  void dfs(int from)
  {
    nodes[from].reach = true;
    for ( auto &v: nodes[from].in_edges )
    {
      if ( nodes[v.first].reach ) continue;
      dfs(v.first);
    }
  }
   // version for sparsed graphs
   // based on set version from https://e-maxx.ru/algo/dijkstra_sparse
   void make_dijkstra(int s)
   {
     set < pair<uint64_t,int> > q;
     dist[s] = 0;
     q.insert (make_pair (dist[s], s));
     while (!q.empty())
     {
	int v = q.begin()->second;
	q.erase (q.begin());
 
	for (auto &e: nodes[v].edges)
        {
          if ( !nodes[e.first].reach ) continue;
	  int to = e.first,
	      len = e.second;
	  if (dist[v] + len < dist[to]) {
	    pair<uint64_t,int> p{ dist[to], to };
	    q.erase ( p );
	    p.first = dist[to] = dist[v] + len;
	    q.insert(p);
	  }
	}
     }
   }
   void dijkstra_rev(int s)
   {
     set < pair<uint64_t,int> > q;
     dist_rev[s] = 0;
     q.insert (make_pair (dist_rev[s], s));
     while (!q.empty())
     {
	int v = q.begin()->second;
	q.erase (q.begin());
 
	for (auto &e: nodes[v].in_edges)
        {
          if ( !nodes[e.first].reach ) continue;
	  int to = e.first,
	      len = e.second;
	  if (dist_rev[v] + len < dist_rev[to]) {
	    pair<uint64_t,int> p{ dist_rev[to], to };
	    q.erase ( p );
	    p.first = dist_rev[to] = dist_rev[v] + len;
	    q.insert(p);
	  }
	}
     }
   }
   int64_t calc()
   {
     int64_t res = LONG_MAX;
     for ( int i = 0; i < N; i++ )
     {
       if ( !nodes[i].reach ) continue;
       for ( auto &e: nodes[i].edges )
       {
         if ( !nodes[e.first].reach ) continue;
         res = min(res, dist[i] + dist_rev[e.first] + e.second / 2);
#ifdef DEBUG
if ( res < 0 ) printf("i %d dist %ld to %d dist %ld\n", i, dist[i], e.first, dist_rev[e.first]);
#endif
       }
     }
     return res;
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
    int a, b, k;
    cin>>a>>b>>k;
    g.add_edge(a, b, k);
  }
printTime("filled");
  g.dfs(n-1);
printTime("dfs");
  g.make_dijkstra(0);
printTime("dijkstra");
  g.dijkstra_rev(n-1);
printTime("dijkstra_rev");
  printf("%ld\n", g.calc());
}