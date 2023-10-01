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
// -- stat for test 3 from new_roads_slow2.cc
// results: 34.196369
// skipped 262292110 max 18510 avg 1311.460550
// visited 1440130646 max 18540 avg 7200.653230
// visited back 228616259 max 18476 avg 1143.081295
// chec-points 67 total size 3448676 nodes, max size 99857, avg size 51472.776119
// --- new results
// CP_MAX 200: 18.848745s check-points 326
// CP_MAX 250: 17.577665s check-points 263
// CP_MAX 300: 18.282396s check-points 221
// CP_MAX 350: 16.517603s check-points 190
// CP_MAX 400: 16.184116s check-points 167
// CP_MAX 450: 16.144981s check-points 150
// CP_MAX 550: 15.762493s check-points 134
// CP_MAX 600: 15.404927s check-points 112

int CP_MAX = 450;
int CHAIN_MAX = 400;

int g_n = 0; // amount of vertices in graph

struct connected;
struct node;

struct parent
{
  parent *p; // null for root
  parent *nc; // ptr to nearest parent contained merging of two connections
  parent *cp; // ptr to nearest parent with check-point
  int day;
  int n = -1;
  int visited = 0; // for queries - path from left node to root
  unordered_set<node *> *check_point = nullptr;
  parent(int d)
  {
    cp = nc = p = nullptr;
    day = d;
  }
  void dump()
  {
    printf("%d day %d\n", n, day);
    for ( parent *pp = p; pp; pp = pp->p )
    {
      if ( pp->visited )
      {
        if ( check_point )
         printf(" cp %p size %ld day %d visited %d nc %p next cp %p\n", 
           pp, pp->check_point->size(), pp->day, pp->visited, pp->nc, pp->cp);
        else
         printf(" day %d visited %d my %p nc %p next cp %p\n", pp->day, pp->visited, pp, pp->nc, pp->cp);
      } else {
        if ( check_point )
           printf(" cp %p size %ld day %d nc %p next cp %p\n", 
             pp, pp->check_point->size(), pp->day, pp->nc, pp->cp);
        else {
          if ( pp->cp )
            printf(" %d day %d nc %p next cp %p\n", pp->n, pp->day, pp->nc, pp->cp);
          else
            printf(" %d day %d nc %p\n", pp->n, pp->day, pp->nc);
        }
      }
    }
  }
};

struct node
{
  int n;
  parent *p = nullptr;
  connected *c = nullptr;
  void dump();
};

#ifdef TIME
int g_cp_count = 0;
int64_t g_cp_size = 0;
int64_t g_cp_size_max = 0;
#endif

struct connected
{
  parent *root = nullptr;
  parent *first = nullptr;
  unordered_set<node *> nodes;
  vector<parent *> cps;
  bool dumped = false;
  void shorting(parent *nc)
  {
    for ( ; first; first = first->p )
      first->nc = nc;
    first = nullptr;
  }
  inline void patch_cps(parent *nc)
  {
    for ( auto c: cps )
      c->cp = nc;
    cps.clear();
  }
  void merge(connected *rhs, parent *nc)
  {
    for ( auto n: rhs->nodes )
    {
      n->c = this;
      nodes.insert(n);
    }
    if ( cps.size() > CP_MAX )
    {
      auto curr_size = nodes.size();
      if ( curr_size < g_n / 2 )
      {
        // make new check-point
        nc->check_point = new unordered_set<node *>(nodes);
#ifdef TIME
        g_cp_count++;
        auto cp_size = nc->check_point->size();
        g_cp_size += cp_size;
        if ( cp_size > g_cp_size_max )
          g_cp_size_max = cp_size;
#endif
        patch_cps(nc);
        rhs->patch_cps(nc);
      }
    }
    if ( !nc->check_point )
    {
      for ( auto c: rhs->cps )
        cps.push_back(c);
    }
    delete rhs;
    cps.push_back(nc);
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
      bn.c->cps.push_back(an.p);
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
      an.c->cps.push_back(bn.p);
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
      an.c->merge(bn.c, cp);
      an.c->root = cp;
      an.c->first = cp;
    } else {
      bn.c->merge(an.c, cp);
      bn.c->root = cp;
      bn.c->first = cp;
    }
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

#ifdef TIME
// some stat about number of processed nodes
int64_t g_skipped = 0;      // amount of skipped cp nodes in skip function, for avg is / q
int g_skipped_max = 0;      // max from skip function
int64_t g_cp_visited = 0;   // amount of visited cp nodes in query, for avg is / q
int g_cp_visited_max = 0;   // max of visited cp nodes in query
int64_t g_visited_back = 0; // amount of visited cp nodes in back path till crossing
int g_visited_back_max = 0; // max of back visited cp nodes
int g_skipped_cp = 0;
int g_back_skipped_cp = 0;
int64_t g_cp_find_ops = 0;
#endif

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

parent *back_from(parent *p, node *a, node *b)
{
  parent *cp = p;
  if ( !cp->check_point )
  {
    cp = cp->nc;
    if ( cp && !cp->check_point )
      cp = cp->cp;
  }
  if ( !cp || !cp->check_point )
    return p;
  auto n1 = cp->check_point->find(a);
  auto n2 = cp->check_point->find(b);
#ifdef TIME
  g_cp_find_ops += 2;
#endif
  if ( n1 == cp->check_point->end() && n2 == cp->check_point->end() )
  {
    printf("back_from bug - cannot find both %d & %d in check-point %p day %d\n", a->n, b->n, cp, cp->day);
    return p;
  }
  if ( n1 != cp->check_point->end() && n2 != cp->check_point->end() )
  {
    return p;
  }
  node *what = nullptr;
  if ( n1 != cp->check_point->end() )
    what = b;
  else
    what = a;
  p = cp;
  for ( ; cp; cp = cp->cp )
  {
    n1 = cp->check_point->find(what);
#ifdef TIME
    g_cp_find_ops++;
#endif
    if ( n1 != cp->check_point->end() )
      return p;
#ifdef TIME
    g_back_skipped_cp++;
#endif
    p = cp;
  }
  return p;
}

parent *skip(parent *p, int d, node *a, node *b, parent **up_to)
{
  parent *cp = p;
  if ( !cp->check_point )
  {
    cp = cp->nc;
    if ( cp && !cp->check_point )
      cp = cp->cp;
  }
//  if ( !cp )
//    p->dump();
  if ( cp && cp->check_point )
  {
    auto n1 = cp->check_point->find(a);
    auto n2 = cp->check_point->find(b);
#ifdef TIME
    g_cp_find_ops += 2;
#endif
    if ( n1 == cp->check_point->end() && n2 == cp->check_point->end() )
    {
      printf("skip bug - cannot find both %d & %d in check-point %p day %d\n", a->n, b->n, cp, cp->day);
    } else
    if ( n1 != cp->check_point->end() && n2 != cp->check_point->end() )
    {
      *up_to = cp;
    } else {
      node *what = nullptr;
      if ( n1 != cp->check_point->end() )
        what = b;
      else
        what = a;
      p = cp;
      for ( ; cp; cp = cp->cp )
      {
        n1 = cp->check_point->find(what);
#ifdef TIME
        g_cp_find_ops++;
#endif
        if ( n1 != cp->check_point->end() )
        {
          *up_to = cp;
          break;
        }
#ifdef TIME
        g_skipped_cp++;
#endif
        p = cp;
      }
    }
  }
  parent *prev = p;
#ifdef TIME
  int cnt = 0;
#endif
  for ( ; p; p = p->nc )
  {
    if ( p->day > d )
      break;
#ifdef TIME
    cnt++;
#endif
    prev = p;
  }
#ifdef TIME
  g_skipped += cnt;
  if ( cnt > g_skipped_max )
    g_skipped_max = cnt;
#endif
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
  parent *up_to = nullptr;
  start = skip(an->p, bn->p->day, an, bn, &up_to);
  parent *par = start;
  if ( -1 != start->n )
    par = par->nc;
#ifdef TIME
  int cnt = 0;
#endif
  for ( parent *ap = par; ap; ap = ap->nc )
  {
#ifdef DEBUG2
printf("mark %p n %d %d day %d\n", ap, ap->n, 1 + c_idx, ap->day);
#endif
#ifdef TIME
    cnt++;
#endif
    g->mark(ap, ++c_idx);
    if ( up_to && up_to == ap )
      break;
  }
#ifdef TIME
  g_cp_visited += cnt;
  if ( cnt > g_cp_visited_max )
    g_cp_visited_max = cnt;
  cnt = 0;
#endif
  // traverse back path
  for ( parent *bp = back_from(bn->p, an, bn); bp; bp = bp->nc )
  {
#ifdef TIME
    cnt++;
#endif
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
#ifdef TIME
  g_visited_back += cnt;
  if ( cnt > g_visited_back_max )
    g_visited_back_max = cnt;
#endif
  // restore visited flag for all left nodes
  g->restore();
  return res;
}

int main(int argc, char **argv)
{
  if ( argc > 1 )
  {
    CP_MAX = atoi(argv[1]);
    // printf("CP_MAX %d\n", CP_MAX); 
  }
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int m, q;
  cin>>g_n>>m>>q;
  graph g(g_n);
  for ( int i = 0; i < m; ++i )
  {
    int a, b;
    cin>>a>>b;
    g.add_edge(a, b, i);
  }
#ifdef TIME
  auto mc_time = getTime();
#endif
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
#ifdef TIME
  printf("make connections: %f\n", mc_time);
  printTime("results");
  printf("skipped %ld max %d avg %f\n", g_skipped, g_skipped_max, (double)g_skipped / q);
  printf("visited %ld max %d avg %f\n", g_cp_visited, g_cp_visited_max, (double)g_cp_visited / q);
  printf("visited back %ld max %d avg %f\n", g_visited_back, g_visited_back_max, (double)g_visited_back / q);
  printf("skipped cp %d, back skipped %d, find ops %ld\n", g_skipped_cp, g_back_skipped_cp, g_cp_find_ops);
  printf("check-points %d total size %ld nodes, max size %ld, avg size %f\n", 
    g_cp_count, g_cp_size, g_cp_size_max, (double)g_cp_size / g_cp_count);
#endif
}
