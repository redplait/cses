#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <limits.h>
#include <string.h>
#include <queue>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <functional>

// solution for Investigation
// All questions can be answered with just one pass of Dijkstra:
// 1) what is the minimum price of such a route? - this is what Dijkstra algo does
// 2) how many minimum-price routes are there? - we can calc amount for each node with the same minimal distance
// 3) what is the minimum number of flights in a minimum-price route? - again we can keep min for each node
// 4) what is the maximum number of flights in a minimum-price route? - keep max for each node too
// all operations except 2) are idemponent. So for 2 order of processing is very important
using namespace std;
// type to hold distances
typedef int64_t CT;
typedef pair<CT, int> Ditem;
const int64_t mod = 1000000007;

struct node
{
  int n;
  int64_t amount;    // for q2
  int min_nodes = 0; // for q3
  int max_nodes = 0; // for q4
  // note that inside each node distance is just int
  vector<pair<int, int> > edges;
  inline int degree() const
  { return (int)edges.size(); }
  inline void put(int v, int w)
  { edges.push_back({ v, w }); }
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
  // and here dist is CT bcs it is sum of many distances from source upto some vertex
  vector<CT> dist;
  // vector<node *> parents;
  int edge_count = 0;
  // char *visited;
  graph(int N)
   : nodes(N), dist(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
    // visited = (char *)malloc(N);
    reset();
  }
  void reset()
  {
    fill(dist.begin(), dist.end(), LONG_MAX);
    // memset(visited, 0, nodes.size());
  }
  inline void add_edge(int a, int b, int k)
  {
    if ( a == b )
      return;
    node &n = nodes[a-1];
    n.put( b-1, k );
    edge_count++;
  }
  // version for sparsed graphs
  // based on set version from https://e-maxx.ru/algo/dijkstra_sparse
  struct order_cmp
  {
    graph *g;
    order_cmp(graph *_g): g(_g) {}
    bool operator() (int a, int b) const
    {
      if ( g->dist[a] == g->dist[b] ) return a < b;
      return g->dist[a] < g->dist[b];
    }
  };
  void make_dijkstra(int s)
  {
    order_cmp cmp(this);
    set <int, order_cmp> q(cmp);
    dist[s] = 0;
    nodes[s].amount = 1;
    q.insert(s);
    while (!q.empty())
    {
        int u = *q.begin();
        CT d = dist[u];
        q.erase(q.begin());
        // visited[u] = true;
        if ( d > dist[u] ) continue;
        for (auto &e: nodes[u].edges )
        {
            int to = e.first;
            if ( to == u ) continue;
            CT w = e.second;
            if ( d + w <= dist[to] )
            {
              // to avoid multiple visiting of node to - remove it from q if already has some distance
              if ( dist[to] != LONG_MAX ) q.erase(to);
              if ( /* !visited[to] && */ d + w < dist[to])
              {
                nodes[to].min_nodes = 1 + nodes[u].min_nodes;
                nodes[to].max_nodes = 1 + nodes[u].max_nodes;
                nodes[to].amount = nodes[u].amount;
                dist[to] = d + w;
                // parents[to] = &nodes[u];
              } else if ( d + w == dist[to] ) // we should ignore visited flag here
              {
                nodes[to].min_nodes = min(nodes[to].min_nodes, 1 + nodes[u].min_nodes);
                nodes[to].max_nodes = max(nodes[to].max_nodes, 1 + nodes[u].max_nodes);
                nodes[to].amount += nodes[u].amount;
                nodes[to].amount %= mod;
              }
              q.insert(to);
            }
        }
     }
   }
   void dump(int v)
   {
     printf("%ld %ld %d %d\n", dist[v], nodes[v].amount, nodes[v].min_nodes, nodes[v].max_nodes);
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
printTime("read");
  g.make_dijkstra(0);
  g.dump(n-1);
printTime("dijkstra");
}