#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

// Round Trip II
// the same as previous solution but for directed graph
// so you can`t reverse order of vertices

using namespace std;

struct node
{
  int n;
  vector<int> edges;
  inline int degree() const
  {
    return edges.size();
  }
  void dump() const
  {
    printf("%d: ", n);
    for ( int e: edges )
      printf("%d ", e);
    printf("\n");
  }
  char color = 0;
};

struct graph
{
  vector<node> nodes;
  graph(int N)
   : nodes(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
  }
  void dump() const
  {
    for ( auto &n: nodes )
     n.dump();
  }
  inline void add_edge(int a, int b)
  {
    node &n = nodes[a-1];
    n.edges.push_back(b-1);
//    node &m = nodes[b-1];
//    m.edges.push_back(a-1);
  }
  void dump_stack(const vector<int> &s) const
  {
    if ( !s.empty() )
      for ( auto i: s )
       printf(" %d", i);
    printf("\n");
  }
  // based on https://neerc.ifmo.ru/wiki/index.php?title=%D0%98%D1%81%D0%BF%D0%BE%D0%BB%D1%8C%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%BE%D0%B1%D1%85%D0%BE%D0%B4%D0%B0_%D0%B2_%D0%B3%D0%BB%D1%83%D0%B1%D0%B8%D0%BD%D1%83_%D0%B4%D0%BB%D1%8F_%D0%BF%D0%BE%D0%B8%D1%81%D0%BA%D0%B0_%D1%86%D0%B8%D0%BA%D0%BB%D0%B0
  // complexity O(V + E)
  void dfs(int v, vector<int> &s)
  {
#ifdef DEBUG
printf("dfs %d:", v); dump_stack(s);
#endif
    nodes[v].color = 1;
    for ( int u: nodes[v].edges )
    {
      if ( !nodes[u].color )
      {
#ifdef DEBUG
printf("u %d\n", u);
#endif
        s.push_back(v);
        dfs(u, s);
        s.pop_back();
      }
#ifdef DEBUG
printf("u2 %d color %d\n", u, nodes[u].color);
#endif
      if ( nodes[u].color == 1 )
      {
        vector<int> res;
        for ( auto si = s.rbegin(); si != s.rend(); ++si )
        {
          res.push_back(*si);
          if ( *si == u )
            break;
        }
        reverse(res.begin(), res.end());
        res.push_back(v); res.push_back(u);
        printf("%ld\n", res.size());
        for ( int p: res )
         printf("%d ", p + 1);
        exit(0);
      }
    }
    nodes[v].color = 2;
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
#ifdef DEBUG
  g.dump();
#endif
  for ( int i = 0; i < n; ++i )
  {
    if ( g.nodes[i].color || !g.nodes[i].degree() )
      continue;
    vector<int> s;
    g.dfs(i, s);
  }
  printf("IMPOSSIBLE");
}