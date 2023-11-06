#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>
#include <set>
// kssp library from https://gitlab.inria.fr/dcoudert/k-shortest-simple-paths
#include "yen.h"

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

int main(int argc, char **argv)
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  directed_graph::DirectedGraph<int,int> g(n);
  for ( int i = 0; i < m; ++i )
  {
    int a, b, k;
    cin>>a>>b>>k;
    g.add_edge(a-1, b-1, k);
  }
#ifdef DEBUG
  printf("order %ld size %ld\n", g.order(), g.size());
#endif
printTime("start");
  kssp::Yen<int, int> kth(&g, 0, n-1);
  auto first = kth.next_path();
  set<int> cands;
  int p_cost = first.second;
  for ( auto v: first.first ) {
    cands.insert(v);
#ifdef DEBUG
 printf("%d ", v);
#endif
  }
  while ( !kth.empty() )
  {
    auto next = kth.next_path();
    if ( next.second != p_cost ) break;
    vector<int> remain;
    for ( auto v: next.first )
    {
      auto ci = cands.find(v);
      if ( ci != cands.end() ) remain.push_back(v);
    }
    cands.clear();
    for ( auto v: remain ) cands.insert(v);
  }
printTime("end");
  printf("size %ld\n", cands.size());
  for ( auto v: cands ) printf("%d ", 1+v);
  printf("\n");
}