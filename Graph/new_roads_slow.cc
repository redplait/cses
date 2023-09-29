#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <unordered_set>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

using namespace std;

// New Roads Queries

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
      printf("c %p size %d ", c, c->nodes.size());
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
  graph(int N)
   : nodes(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
  }
  void dump()
  {
    for ( auto &n: nodes )
     n.dump();
  }
  inline void add_edge(int a, int b, int days)
  {
    node &an = nodes[a-1];
    node &bn = nodes[b-1];
    if ( !an.c && !bn.c ) // just edge between some unconnected nodes
    {
      connected *cc = new connected();
      an.c = bn.c = cc;
      cc->nodes.insert(&an); cc->nodes.insert(&bn);
      // make common parent
      cc->root = new parent(days + 1);
      an.p = bn.p = cc->root;
      return;
    }
    // check if we try add edge to the same connection set
    if ( an.c == bn.c )
      return;
    if ( !an.c ) // add node an to connection set in bn
    {
      an.p = new parent(days + 1);
      an.c = bn.c;
      an.c->nodes.insert(&an);
      bn.c->root->p = an.p;
      bn.c->root = an.p;
      return;
    }
    if ( !bn.c) // add node bn to connection set in an
    {
      bn.p = new parent(days + 1);
      bn.c = an.c;
      bn.c->nodes.insert(&bn);
      an.c->root->p = bn.p;
      an.c->root = bn.p;
      return;
    }
    // merge two connection sets
    auto cp = new parent(days + 1);
    an.c->root->p = cp;
    bn.c->root->p = cp;
    if ( an.c->nodes.size() > bn.c->nodes.size() )
    {
      an.c->merge(bn.c);
      an.c->root = cp;
    } else {
      bn.c->merge(an.c);
      bn.c->root = cp;
    }
  }
};

inline void printTime(const char *pfx)
{
#ifdef TIME
  printf("%s: %f\n", pfx, (float)clock()/CLOCKS_PER_SEC);
#endif
}

int query(graph *g, int a, int b)
{
  auto *an = &g->nodes[a-1];
  auto *bn = &g->nodes[b-1];
  if ( !an->c || !bn->c )
    return -1;
  if ( an->c != bn->c )
    return -1;
  // ok, we know than cities a & b belong to the same connected set, lets see when they became connected
  int res = 0;
  // for left node mark all parent as visited
  for ( parent *ap = an->p; ap; ap = ap->p )
    ap->visited = 1;
  // for right node find first visited parent
  for ( parent *bp = bn->p; bp; bp = bp->p )
  {
    if ( bp->visited )
    {
      res = bp->day;
      break;
    }
    if ( !bp->p )
      res = bp->day;
  }
  // restore visited flag for all left nodes
  for ( parent *ap = an->p; ap; ap = ap->p )
    ap->visited = 0;
  return res;
}

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m, q;
  cin>>n>>m>>q;
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
  for ( int i = 0; i < q; ++i )
  {
    int a, b;
    cin>>a>>b;
    printf("%d\n", query(&g, a, b));
  }
  printTime("results");
}
