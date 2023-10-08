#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <unordered_set>

// Flight Routes Check

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

struct node
{
  int n;
  unordered_set<int> in_edges;
  unordered_set<int> edges;
};

struct graph
{
  vector<node> nodes;
  char *visited;
  graph(int N)
   : nodes(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
    visited = (char *)malloc(N);
    reset();
  }
  void reset()
  {
    memset(visited, 0, nodes.size());
  }
  inline void add_edge(int a, int b)
  {
    if ( a == b )
      return;
    node &n = nodes[a-1];
    n.edges.insert(b-1);
    node &m = nodes[b-1];
    m.in_edges.insert(a-1);
  }
  void check(int to, int arrow)
  {
    for ( size_t i = 0; i < nodes.size(); i++ )
    {
      if ( !visited[i] )
      {
        puts("NO");
        if ( !arrow )
          printf("%ld %d\n", i + 1, to + 1);
        else
          printf("%d %ld\n", to + 1, i + 1);
        exit(0);
      }
    }
  }
  void FlightRouteDFS(int a, int arrow)
  {
    if (visited[a]) return;
    visited[a] = true;
    for (int b : arrow ? nodes[a].edges : nodes[a].in_edges )
      FlightRouteDFS(b, arrow);
  }
  void calc_connectivity()
  {
    FlightRouteDFS(0, 0);
    check(0, 0);
    reset();
    FlightRouteDFS(0, 1);
    check(0, 1);
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
    int a, b;
    cin>>a>>b;
    g.add_edge(a, b);
  }
  g.calc_connectivity();
  puts("YES");
}