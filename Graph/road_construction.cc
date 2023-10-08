#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <limits.h>
#include <vector>
#include <unordered_set>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

using namespace std;

// Road Construction

struct parent
{
  parent *p; // null for root
  int day;
  bool visited = false; // for queries - path from left node to root
  parent(int d)
  {
    p = nullptr;
    day = d;
  }
};

struct connected;

struct node
{
  int n;
  parent *p = nullptr;
  connected *c = nullptr;
  void dump();
};

struct connected
{
  parent *root;
  unordered_set<node *> nodes;
  bool dumped = false;
  void merge(connected *rhs)
  {
    for ( auto n: rhs->nodes )
    {
      n->c = this;
      nodes.insert(n);
    }
    delete rhs;
  }
  connected &operator+=(connected &rhs)
  {
    for ( auto n: rhs.nodes )
    {
      n->c = this;
      nodes.insert(n);
    }
    return *this;
  }
};

void node::dump()
{
  printf("%d: ", n);
  if ( p )
  {
    if ( !c->dumped )
    {
      printf("c %p size %ld ", c, c->nodes.size());
      for ( const parent *pi = p; pi; pi = pi->p )
        printf("%d ", pi->day);
      c->dumped = true;
    }
  }
  printf("\n");
}

struct graph
{
  vector<node> nodes;
  int comp_no;
  int max_comp = INT_MIN;
  graph(int N)
   : nodes(N), comp_no(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
  }
  void dump()
  {
    for ( auto &n: nodes )
     n.dump();
  }
  inline void day()
  { printf("%d %d\n", comp_no, max_comp); }
  inline void add_edge(int a, int b, int days)
  {
    node &an = nodes[a-1];
    node &bn = nodes[b-1];
    if ( !an.c && !bn.c ) // just edge between some unconnected nodes
    {
      connected *cc = new connected();
      comp_no--; // +1 comp -2 node
      an.c = bn.c = cc;
      cc->nodes.insert(&an); cc->nodes.insert(&bn);
      if ( (int)cc->nodes.size() > max_comp ) max_comp = (int)cc->nodes.size();
      // make common parent
      cc->root = new parent(days + 1);
      an.p = bn.p = cc->root;
      day();
      return;
    }
    // check if we try add edge to the same connection set
    if ( an.c == bn.c )
    {
      day();
      return;
    }
    if ( !an.c ) // add node an to connection set in bn
    {
      comp_no--;
      an.p = new parent(days + 1);
      an.c = bn.c;
      an.c->nodes.insert(&an);
      if ( (int)an.c->nodes.size() > max_comp ) max_comp = (int)an.c->nodes.size();
      bn.c->root->p = an.p;
      bn.c->root = an.p;
      day();
      return;
    }
    if ( !bn.c) // add node bn to connection set in an
    {
      comp_no--;
      bn.p = new parent(days + 1);
      bn.c = an.c;
      bn.c->nodes.insert(&bn);
      if ( (int)bn.c->nodes.size() > max_comp ) max_comp = (int)bn.c->nodes.size();
      an.c->root->p = bn.p;
      an.c->root = bn.p;
      day();
      return;
    }
    // merge two connection sets
    comp_no--;
    auto cp = new parent(days + 1);
    an.c->root->p = cp;
    bn.c->root->p = cp;
    if ( an.c->nodes.size() > bn.c->nodes.size() )
    {
      an.c->merge(bn.c);
      if ( (int)an.c->nodes.size() > max_comp ) max_comp = (int)an.c->nodes.size();
      an.c->root = cp;
    } else {
      bn.c->merge(an.c);
      if ( (int)bn.c->nodes.size() > max_comp ) max_comp = (int)bn.c->nodes.size();
      bn.c->root = cp;
    }
    day();
  }
};

inline void printTime(const char *pfx)
{
#ifdef TIME
  printf("%s: %f\n", pfx, (float)clock()/CLOCKS_PER_SEC);
#endif
}

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
    g.add_edge(a, b, i);
  }
  printTime("make connections");
#ifdef DEBUG
  g.dump();
#endif
  printTime("results");
}
