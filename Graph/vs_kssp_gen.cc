#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>
#include <set>
// kssp library from https://gitlab.inria.fr/dcoudert/k-shortest-simple-paths
#include "yen.h"
#include "sidetrack_based.h"
#include "node_classification.h"
#include "postponed_node_classification.h"
#include "postponed_node_classification_star.h"
#include "parsimonious_sidetrack_based.h"

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

struct IKth
{
  virtual bool run(set<int> &) = 0;
  virtual ~IKth() {}
  size_t paths = 0;
};

template <typename T>
struct Kth: public IKth
{
  T *w = nullptr;
  Kth(directed_graph::DirectedGraph<int,int> *g, int ssource, int ttarget)
  {
    w = new T(g, ssource, ttarget);
  }
  Kth(directed_graph::DirectedGraph<int,int> *g, int ssource, int ttarget, int version)
  {
    w = new T(g, ssource, ttarget, version, false);
  }
  virtual bool run(set<int> &res)
  {
    res.clear();
    auto first = w->next_path();
    int p_cost = first.second;
    if ( !p_cost ) return false;
    for ( auto v: first.first ) res.insert(v);
    while ( !w->empty() )
    {
      auto next = w->next_path();
      if ( next.second != p_cost ) break;
      paths++;
      vector<int> remain;
      for ( auto v: next.first )
      {
        auto ci = res.find(v);
        if ( ci != res.end() ) remain.push_back(v);
      }
      res.clear();
      for ( auto v: remain ) res.insert(v);
    }
    return true;
  }
  virtual ~Kth()
  {
    if ( w ) delete w;
  }
};

void usage(const char *me)
{
  printf("Usage %s: [algo num]\n", me);
  puts("algos");
  puts("1 - Yen (default)");
  puts("2 - Node Classification");
  puts("3 - Postponed Node Classification");
  puts("4 - PNC*");
  puts("5 - Parsimonious Sidetrack Based");
  puts("6 - Parsimonious Sidetrack Based v2");
  puts("7 - Parsimonious Sidetrack Based v3");
  puts("8 - Sidetrack Based");
  puts("9 - Sidetrack Based with updates");
  exit(8);
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
  IKth *kth;
  if ( argc > 1 )
  {
    int what = atoi(argv[1]);
    if ( !what ) usage(argv[0]);
    switch(what)
    {
      case 1: kth = new Kth<kssp::Yen<int, int> >(&g, 0, n-1); break;
      case 2: kth = new Kth<kssp::NodeClassification<int, int> >(&g, 0, n-1); break;
      case 3: kth = new Kth<kssp::PostponedNodeClassification<int, int> >(&g, 0, n-1); break;
      case 4: kth = new Kth<kssp::PostponedNodeClassificationStar<int, int> >(&g, 0, n-1); break;
      case 5: kth = new Kth<kssp::ParsimoniousSidetrackBased<int, int> >(&g, 0, n-1, 1); break;
      case 6: kth = new Kth<kssp::ParsimoniousSidetrackBased<int, int> >(&g, 0, n-1, 2); break;
      case 7: kth = new Kth<kssp::ParsimoniousSidetrackBased<int, int> >(&g, 0, n-1, 3); break;
      case 8: kth = new Kth<kssp::SidetrackBased<int, int> >(&g, 0, n-1, 1); break;
      case 9: kth = new Kth<kssp::SidetrackBased<int, int> >(&g, 0, n-1, 4); break;
      default: usage(argv[0]);
    }
  } else
    kth = new Kth<kssp::Yen<int, int> >(&g, 0, n-1);
  set<int> cands;
  kth->run(cands);
printTime("end");
  printf("size %ld paths %ld\n", cands.size(), kth->paths);
  for ( auto v: cands ) printf("%d ", 1+v);
  printf("\n");
  delete kth;
}