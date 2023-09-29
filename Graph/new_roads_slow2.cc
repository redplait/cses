#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <unordered_set>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

using namespace std;

// New Roads Queries
// on test 2 works in (making connections in 0.025s) 0.518s vs 2.227s from first version
// on test 3 works in 28.29s vs 497.46s from first version

struct parent
{
  parent *p; // null for root
  parent *nc; // ptr to nearest parent contained merging of two connections
  int day;
  int n = -1;
  int visited = 0; // for queries - path from left node to root
  parent(int d)
  {
    nc = p = nullptr;
    day = d;
  }
  void dump()
  {
    printf("%d day %d\n", n, day);
    for ( parent *pp = p; pp; pp = pp->p )
    {
      if ( pp->visited )
        printf(" day %d visited %d my %p nc %p\n", pp->day, pp->visited, pp, pp->nc);
      else
        printf(" %d day %d nc %p\n", pp->n, pp->day, pp->nc);
    }
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
  parent *root = nullptr;
  parent *first = nullptr;
  unordered_set<node *> nodes;
  bool dumped = false;
  void shorting(parent *nc)
  {
    for ( ; first; first = first->p )
      first->nc = nc;
    first = nullptr;
  }
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
  if ( !p )
    printf("%d no parent", n);
  else
    printf("%d day %d: ", n, p->day);
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
  parent **visited;
  int visited_idx;
  graph(int N)
   : nodes(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
    visited = (parent **)malloc( (N - 1) * sizeof(parent *) );
    visited_idx = 0;
  }
  ~graph()
  {
    if ( visited )
      free(visited);
  }
  inline void mark(parent *p, int m)
  {
    if ( p->n != -1 )
      return;
    p->visited = m;
    visited[visited_idx++] = p;
  }
  inline parent* top()
  {
    if ( !visited_idx )
      return nullptr;
    return visited[visited_idx - 1];
  }
  void restore()
  {
    for ( int i = 0; i < visited_idx; ++i )
      visited[i]->visited = false;
    visited_idx = 0;
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
      cc->first = an.p = bn.p = cc->root;
      return;
    }
    // check if we try add edge to the same connection set
    if ( an.c == bn.c )
      return;
    if ( !an.c ) // add node an to connection set in bn
    {
      an.p = new parent(days + 1);
      if ( !bn.c->first )
        bn.c->first = an.p;
      an.c = bn.c;
      an.p->n = an.n;
      an.c->nodes.insert(&an);
      bn.c->root->p = an.p;
      bn.c->root = an.p;
      return;
    }
    if ( !bn.c) // add node bn to connection set in an
    {
      bn.p = new parent(days + 1);
      if ( !an.c->first )
        an.c->first = bn.p;
      bn.c = an.c;
      bn.p->n = bn.n;
      bn.c->nodes.insert(&bn);
      an.c->root->p = bn.p;
      an.c->root = bn.p;
      return;
    }
    // merge two connection sets
    auto cp = new parent(days + 1);
    an.c->shorting(cp);
    bn.c->shorting(cp);
    an.c->root->nc = bn.c->root->nc = cp;
    an.c->root->p = cp;
    bn.c->root->p = cp;
    if ( an.c->nodes.size() > bn.c->nodes.size() )
    {
      an.c->merge(bn.c);
      an.c->root = cp;
      an.c->first = cp;
    } else {
      bn.c->merge(an.c);
      bn.c->root = cp;
      bn.c->first = cp;
    }
  }
};

inline void printTime(const char *pfx)
{
#ifdef TIME
  printf("%s: %f\n", pfx, (float)clock()/CLOCKS_PER_SEC);
#endif
}

void restore(parent *from)
{
  if ( !from )
    return;
  for ( ; from; from = from->p )
  {
    if ( !from->visited )
      return;
    from->visited = 0;
  }
}

parent *skip(parent *p, int d)
{
  parent *prev = p;
  for ( ; p; p = p->nc )
  {
    if ( p->day > d )
      break;
    prev = p;
  }
  for ( p = prev; p; p = p->p )
  {
    if ( p->day >= d )
      return p;
    prev = p;
  }
  return prev;
}

int query(graph *g, int a, int b)
{
  auto *an = &g->nodes[a-1];
  auto *bn = &g->nodes[b-1];
  if ( an == bn )
    return 0;
  if ( !an->c || !bn->c )
    return -1;
  if ( an->c != bn->c )
    return -1;
  if ( an->p == bn->p )
    return an->p->day;
  // ok, we know than cities a & b belong to the same connected set, lets see when they became connected
  // main idea is that we need to traverse only connection merge points - so iterate on parent->nc
  // at some point path from a and b will cross - let say in C
  // then we need to find in g->visited previous merge point and traverse from it till C
  int c_idx = 0, res = 0;
  parent *start, *last;
  if ( !(an->p->day < bn->p->day) )
  {
    auto old = bn;
    bn = an;
    an = old;
    // swap(an, bn);
  }
  start = skip(an->p, bn->p->day);
  parent *par = start;
  if ( -1 != start->n )
    par = par->nc;
  for ( parent *ap = par; ap; ap = ap->nc )
  {
#ifdef DEBUG2
printf("mark %p n %d %d day %d\n", ap, ap->n, 1 + c_idx, ap->day);
#endif
    g->mark(ap, ++c_idx);
  }
  for ( parent *bp = bn->p; bp; bp = bp->nc )
  {
    if ( !bp->visited )
      continue;
#ifdef DEBUG2
  printf("bp %p %d nc %p visited %d\n", bp, bp->n, bp->nc, bp->visited);
#endif
    if ( bp->visited == 1 )
    {
      for ( parent *ap = start; ap; ap = ap->p )
      {
        if ( ap->n == bn->n )
        {
#ifdef DEBUG2
printf("match ap->n %d\n", ap->n);
#endif
          res = ap->day;
          goto out;
        }
        if ( ap->visited )
        {
#ifdef DEBUG2
printf("last in start chain, visited %d\n", ap->visited);
#endif
           res = ap->day;
           goto out;
        }
      }
    }
    parent *ap;
#ifdef DEBUG2
printf("prev visited %p\n", g->visited[bp->visited - 2]);
#endif
    ap = g->visited[bp->visited - 2]->p;
    if ( !ap )
    {
#ifdef DEBUG2
printf("cannot find visited %d\n", bp->visited);
#endif
      res = g->visited[bp->visited - 2]->day;
      goto out;
    }
#ifdef DEBUG2
printf("ap %p a.n %d b.n %d day %d\n", ap, ap->n, bn->n, ap->day);
#endif
    for ( parent *aa = ap; aa; aa = aa->p )
    {
      if ( aa->n == bn->n )
      {
#ifdef DEBUG2
printf("match aa->n %d\n", aa->n);
#endif
        res = aa->day;
        goto out;
      }
      if ( aa->visited )
      {
#ifdef DEBUG2
printf("aa %p visited %d\n", aa, aa->visited);
#endif
        res = aa->day;
        goto out;
      }
    }
  }
  last = g->top();
#ifdef DEBUG2
 if ( last )
   printf("not found %p visited for %d, top %d\n", bn->p, bn->n, g->top()->visited);
 else
   printf("not found %p visited for %d\n", bn->p, bn->n);
  bn->p->dump();
#endif
  // if we here we need traverse from last visited till end
  for ( parent *ap = last ? last : bn->p; ap; ap = ap->p )
  {
    if ( ap->n == bn->n )
    {
      res = ap->day;
      goto out;
    }
    if ( !ap->p )
    {
      res = ap->day;
      goto out;
    }
  }
out:
  // restore visited flag for all left nodes
  g->restore();
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
#ifdef TEST
  ifstream ti("test_output.txt");
#endif
  for ( int i = 0; i < q; ++i )
  {
    int a, b;
    cin>>a>>b;
    int ans = query(&g, a, b);
    printf("%d\n", ans);
#ifdef TEST
    int mb;
    ti>>mb;
    if ( ans != mb )
    {
      printf("wrong answer %d for %d %d, must be %d\n", ans, a, b, mb);
    }
#endif
  }
  printTime("results");
}
