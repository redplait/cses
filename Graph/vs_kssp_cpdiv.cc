#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
// kssp library from https://gitlab.inria.fr/dcoudert/k-shortest-simple-paths
#include "yen.h"
#include "sidetrack_based.h"
#include "node_classification.h"
#include "postponed_node_classification.h"
#include "postponed_node_classification_star.h"
#include "parsimonious_sidetrack_based.h"

// second try to use kssp
// I made wrong assumption to reduce sizes of input data dividing it on cut-points
// Why it is wrong - lets assume that we have only 2 shortest paths withoud sharing any common vertex (sure except source and target)
// then cut-poiint from first path does not affect second and vice versa
// Anyway kssp even on such reduced input data is too slow and again shows different results
// see details here: http://redplait.blogspot.com/2023/11/kssp-library-part-2.html

using namespace std;
typedef int64_t CT;

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

struct cutpoints
{
  directed_graph::DirectedGraph<int,CT> *m_g;
  vector<int> tin, fup;
  vector<char> visited;
  int timer;
  unordered_map<int, vector<int> > res;
  cutpoints(directed_graph::DirectedGraph<int,CT> *g):
    m_g(g), tin(g->order()), fup(g->order()), visited(g->order()), timer(0)
  {}
  void filter_res(const vector<int> &path)
  {
    unordered_set<int> tmp;
    for ( auto cit = path.cbegin(); cit != path.end(); ++cit )
      tmp.insert(*cit);
    for ( auto it = res.begin(); it != res.end(); )
    {
      if ( none_of(it->second.begin(), it->second.end(), [&tmp](int v) { auto tit = tmp.find(v); return tit != tmp.end(); }) )
        it = res.erase(it);
      else
        ++it;
    }
  }
  // based on https://e-maxx.ru/algo/cutpoints
  // complexity O(V + E)
  void dfs(int v, int p = -1)
  {
    visited[v] = 1;
    tin[v] = fup[v] = timer++;
    int children = 0;
    for ( auto const &e: m_g->out_neighbors[v] ) 
    {
      int to = e.first;
      if (to == p) // cycle to itself
        continue;
      if (visited[to])
        fup[v] = min(fup[v], tin[to]);
      else 
      {
        dfs(to, v);
        fup[v] = min(fup[v], fup[to]);
        if (fup[to] >= tin[v] && p != -1)
          res[v].push_back(to);
        ++children;
      }
    }
    if (p == -1 && children > 1)
      res[v].push_back(v); // root will ref to itself
  }
};

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
  Kth(directed_graph::DirectedGraph<int,CT> *g, int ssource, int ttarget)
  {
    w = new T(g, ssource, ttarget);
  }
  Kth(directed_graph::DirectedGraph<int,CT> *g, int ssource, int ttarget, int version)
  {
    w = new T(g, ssource, ttarget, version, false);
  }
  virtual bool run(set<int> &res)
  {
    auto first = w->next_path();
    CT p_cost = first.second;
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
  directed_graph::DirectedGraph<int,CT> g(n);
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
  int what = 0;
  if ( argc > 1 )
  {
    what = atoi(argv[1]);
    if ( !what ) usage(argv[0]);
  }
  auto get_kth = [what, argv, &g](int from, int to) -> IKth* {
    switch(what)
    {
      case 0:
      case 1: return new Kth<kssp::Yen<int, CT> >(&g, from, to); break;
      case 2: return new Kth<kssp::NodeClassification<int, CT> >(&g, from, to); break;
      case 3: return new Kth<kssp::PostponedNodeClassification<int, CT> >(&g, from, to); break;
      case 4: return new Kth<kssp::PostponedNodeClassificationStar<int, CT> >(&g, from, to); break;
      case 5: return new Kth<kssp::ParsimoniousSidetrackBased<int, CT> >(&g, from, to, 1); break;
      case 6: return new Kth<kssp::ParsimoniousSidetrackBased<int, CT> >(&g, from, to, 2); break;
      case 7: return new Kth<kssp::ParsimoniousSidetrackBased<int, CT> >(&g, from, to, 3); break;
      case 8: return new Kth<kssp::SidetrackBased<int, CT> >(&g, from, to, 1); break;
      case 9: return new Kth<kssp::SidetrackBased<int, CT> >(&g, from, to, 4); break;
      default: usage(argv[0]);
    }
    return nullptr;
  };
  set<int> cands;
  cutpoints cp(&g);
  cp.dfs(0);
printTime("cutpoints");
  if ( cp.res.size() > 2 )
  {
    printf("cutpoint: %ld\n", cp.res.size());
#ifdef DEBUG
    for ( auto i: cp.res ) printf("%d ", i+1);
    printf("\n");
#endif
    // make path with Dijkstra
    dijkstra::Dijkstra d(&g, 0, false);
    d.run(n-1);
    auto path = d.get_path(n-1);
    cp.filter_res(path);
  printf("filtered cutpoint: %ld\n", cp.res.size());
    int prev_cp = 0;
    cands.insert(prev_cp);
    // lets traverse found shortest path and divide it on cutpoint, all vertices between cutpoints store to curr
    set<int> curr;
    int short_cycles = 0;
    for ( int i = 1; i < path.size(); ++i )
    {
      auto is_cp = cp.res.find(path[i]);
      if ( is_cp == cp.res.end() ) { curr.insert(path[i]); continue; }
// #ifdef DEBUG
      printf("%d - %d size %ld\n", prev_cp + 1, path[i] + 1, curr.size());
// #endif
      if ( curr.empty() )
      {
        prev_cp = path[i]; cands.insert(prev_cp);
        continue;
      }
      short_cycles++;
      curr.insert(prev_cp); curr.insert(path[i]);
      IKth *kth = get_kth(prev_cp, path[i]);
      kth->run(curr);
      for ( auto survived: curr ) cands.insert(survived);
      delete kth;
      prev_cp = path[i];
      curr.clear();
    }
    if ( !curr.empty() )
    {
      short_cycles++;
      curr.insert(prev_cp);
      IKth *kth = get_kth(prev_cp, n-1);
      kth->run(curr);
      for ( auto survived: curr ) cands.insert(survived);
      delete kth;
    }
    // insert target
    cands.insert(n-1);
printTime("divided end");
    printf("size %ld cycles %d\n", cands.size(), short_cycles);
    for ( auto v: cands ) printf("%d ", 1+v);
    printf("\n");
  } else {
    IKth *kth = get_kth(0, n-1);
    kth->run(cands);
printTime("end");
    printf("size %ld paths %ld\n", cands.size(), kth->paths);
    for ( auto v: cands ) printf("%d ", 1+v);
    printf("\n");
    delete kth;
  }
}