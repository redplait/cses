#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <unordered_set>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")
#pragma GCC optimize("-fomit-frame-pointer")

using namespace std;

// New Roads Queries

int MAX_BSET = 250;
int CP_MAX = 997;
int CHAIN_MAX = 1010;

int g_n = 0; // amount of vertices in graph
int g_bset_size = 0;
int g_bset_idx = 0;
unsigned char *g_bsets = nullptr;

inline unsigned char *get_bset()
{
  if ( g_bset_idx >= MAX_BSET )
  {
    printf("bsets limit\n");
    exit(3);
  }
  return g_bsets + g_bset_size * (g_bset_idx++);
}

void setup()
{
  int s32 = g_n << 5; // div 32
  if ( g_n & 0x1f )
    s32++;
  g_bset_size = (s32 >> 2);
  size_t bsize = g_bset_size * MAX_BSET;
  g_bsets = (unsigned char *)malloc(bsize);
  memset(g_bsets, 0, bsize);
}

struct connected;
struct node;
struct graph;

struct parent
{
  parent *p; // null for root
  parent *nc; // ptr to nearest parent contained merging of two connections
  parent *cp; // ptr to nearest parent with check-point
  unsigned char *check_point = nullptr;
  int day;
  int32_t visited = 0; // for queries - path from left node to root
  int32_t n = -1;
  parent(int32_t d)
  {
    cp = nc = p = nullptr;
    day = d;
  }
  void dump();
};

struct node
{
  parent *p = nullptr;
  connected *c = nullptr;
  int32_t n;
  void dump();
};

unsigned char *bset_reset()
{ return get_bset(); }

unsigned char *make_Bset(unsigned char *rhs)
{
  unsigned char *b = (unsigned char *)malloc(g_bset_size);
  memcpy(b, rhs, g_bset_size);
  return b;
}
inline void bset_set(unsigned char *b, int i)
{
  int idx = i >> 3; // div 8
  int mask = i & 7;
  b[idx] |= 1 << mask;
}
unsigned char *make_Bset(unordered_set<node *> &rhs)
{
  unsigned char *b = bset_reset();
  for ( auto n: rhs )
    bset_set(b, n->n);
  return b;
}
unsigned char *make_Bset(unordered_set<int32_t> &rhs)
{
  unsigned char *b = bset_reset();
  for ( auto n: rhs )
    bset_set(b, n);
  return b;
}
void or_with(unsigned char *b, unsigned char *rhs)
{
  uint32_t *r = (uint32_t *)rhs;
  for ( uint32_t *p = (uint32_t *)b; p < (uint32_t *)(b + g_bset_size); ++p, ++r )
    *p |= *r;   
}
inline bool bset_test(unsigned char *b, int i)
{
  int idx = i >> 3; // div 8
  int mask = i & 7;
  return b[idx] & (1 << mask);
}

void parent::dump()
{
    printf("%d day %d\n", n, day);
    int cnt = 0;
    for ( parent *pp = p; pp; pp = pp->p, ++cnt )
    {
      if ( pp->visited )
      {
        if ( check_point )
         printf(" cp %p day %d visited %d nc %p next cp %p\n", 
           pp, pp->day, pp->visited, pp->nc, pp->cp);
        else
         printf(" day %d visited %d my %p nc %p next cp %p\n", pp->day, pp->visited, pp, pp->nc, pp->cp);
      } else {
        if ( check_point )
           printf(" cp %p day %d nc %p next cp %p\n", 
             pp, pp->day, pp->nc, pp->cp);
        else {
          if ( pp->cp )
            printf(" %d day %d nc %p next cp %p\n", pp->n, pp->day, pp->nc, pp->cp);
          else
            printf(" %d day %d nc %p\n", pp->n, pp->day, pp->nc);
        }
      }
    }
    printf(" total %d parents\n", cnt);
}

#ifdef TIME
int g_cp_count = 0;
int g_cp_cloned = 0;
int64_t g_cp_size = 0;
int64_t g_cp_size_max = 0;
#endif

struct connected
{
  parent *root = nullptr;
  parent *first = nullptr;
  unordered_set<int32_t> nodes;
  vector<parent *> cps;
  unsigned char *prev_cp = nullptr;
  int prev_conn = 0;
#ifdef DEBUG
  bool dumped = false;
#endif
  void shorting(parent *nc)
  {
    for ( ; first; first = first->p )
      first->nc = nc;
    first = nullptr;
  }
  void merge_nodes(graph *g, connected *rhs);
  inline void patch_cps(parent *nc)
  {
    for ( auto c: cps )
      c->cp = nc;
    cps.clear();
  }
  void make_check_point(parent *nc);
  void make_from_prev(parent *nc);
  void make_chain_cp(parent *nc)
  {
    if ( prev_cp )
      make_from_prev(nc);
    else
      make_check_point(nc);
    cps.push_back(nc);
    prev_cp = nc->check_point;
  }
  void merge2(graph *g, connected *rhs, parent *nc);
  void merge(graph *g, connected *rhs, parent *nc);
};

#ifdef DEBUG
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
#endif

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
      visited[i]->visited = 0;
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
      cc->nodes.insert(an.n); cc->nodes.insert(bn.n);
      // make common parent
      cc->root = new parent(days + 1);
      cc->cps.push_back(cc->root);
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
      an.c->nodes.insert(an.n);
      bn.c->root->p = an.p;
      bn.c->root = an.p;
      if ( !bn.c->prev_conn && bn.c->cps.size() > CHAIN_MAX )
        bn.c->make_chain_cp(an.p);
      else
        bn.c->cps.push_back(an.p);
      return;
    }
    if ( !bn.c ) // add node bn to connection set in an
    {
      bn.p = new parent(days + 1);
      if ( !an.c->first )
        an.c->first = bn.p;
      bn.c = an.c;
      bn.p->n = bn.n;
      bn.c->nodes.insert(bn.n);
      an.c->root->p = bn.p;
      an.c->root = bn.p;
      if ( !an.c->prev_conn && an.c->cps.size() > CHAIN_MAX )
        an.c->make_chain_cp(bn.p);
      else
        an.c->cps.push_back(bn.p);
      return;
    }
    // merge two connection sets
    auto cp = new parent(days + 1); 
    an.c->shorting(cp);
    bn.c->shorting(cp);
    an.c->root->nc = bn.c->root->nc = cp;
    an.c->root->p = bn.c->root->p = cp;
    if ( an.c->nodes.size() > bn.c->nodes.size() )
    {
      if ( an.c->prev_cp && bn.c->prev_cp )
        an.c->merge2(this, bn.c, cp);
      else 
        an.c->merge(this, bn.c, cp);
      an.c->root = cp;
      an.c->first = cp;
      an.c->prev_cp = cp->check_point;
      an.c->prev_conn = 1;
    } else {
      if ( an.c->prev_cp && bn.c->prev_cp )
        bn.c->merge2(this, an.c, cp);
      else 
        bn.c->merge(this, an.c, cp);
      bn.c->root = cp;
      bn.c->first = cp;
      bn.c->prev_cp = cp->check_point;
      bn.c->prev_conn = 1;
    }
  }
};

void connected::merge_nodes(graph *g, connected *rhs)
{
  for ( auto n: rhs->nodes )
  {
    g->nodes[n].c = this;
    nodes.insert(n);
  }
}

void connected::make_from_prev(parent *nc)
{
  nc->check_point = make_Bset(prev_cp);
#ifdef TIME
  g_cp_count++; g_cp_cloned++;
  auto cp_size = nodes.size();
  g_cp_size += cp_size;
  if ( cp_size > g_cp_size_max )
    g_cp_size_max = cp_size;
#endif
  for ( auto c: cps )
  {
    c->cp = nc;
    if ( c->n != -1 )
      bset_set(nc->check_point, c->n);
  }
  if ( nc->n != -1 )
    bset_set(nc->check_point, nc->n);
  cps.clear();
}

void connected::make_check_point(parent *nc)
{
  nc->check_point = make_Bset(nodes);
#ifdef TIME
  g_cp_count++;
  auto cp_size = nodes.size();
  g_cp_size += cp_size;
  if ( cp_size > g_cp_size_max )
    g_cp_size_max = cp_size;
#endif
  patch_cps(nc);
}

void connected::merge2(graph *g, connected *rhs, parent *nc)
{
  merge_nodes(g, rhs);
  if ( cps.size() + rhs->cps.size() > (size_t)CP_MAX )
  {
    make_from_prev(nc);
    or_with(nc->check_point, rhs->prev_cp);
    for ( auto c: rhs->cps )
    {
      c->cp = nc;
      if ( c->n != -1 )
        bset_set(nc->check_point, c->n);
    }
  }
  if ( !nc->check_point )
  {
    for ( auto c: rhs->cps )
      cps.push_back(c);
  } else
    prev_cp = nc->check_point;
  delete rhs;
  cps.push_back(nc);
}

void connected::merge(graph *g, connected *rhs, parent *nc)
{
  merge_nodes(g, rhs);
  if ( cps.size() + rhs->cps.size() > (size_t)CP_MAX )
  {
    make_check_point(nc);
    rhs->patch_cps(nc);
  }
  if ( !nc->check_point )
  {
    for ( auto c: rhs->cps )
      cps.push_back(c);
  } else
    prev_cp = nc->check_point;
  delete rhs;
  cps.push_back(nc);
}

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
int g_no_skips = 0;
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
  for ( ; from; from = from->p )
  {
    if ( !from->visited )
      return;
    from->visited = 0;
  }
}

parent *get_cp(parent *p)
{
  if ( !p )
    return p;
  if ( p->check_point )
    return p;
  if ( p->cp )
    return p->cp;
  if (!p->nc)
    return p;
  return p->nc->cp;
}

parent *back_from(parent *p, node *a, node *b)
{
  parent *cp = get_cp(p);
  if ( !cp || !cp->check_point )
  {
#ifdef TIME
    g_no_skips++;
#endif
//    p->dump();
    return p;
  }
  auto n1 = bset_test(cp->check_point, a->n);
  auto n2 = bset_test(cp->check_point, b->n);
#ifdef TIME
  g_cp_find_ops += 2;
#endif
  if ( !n1 && !n2 )
  {
    printf("back_from bug - cannot find both %d & %d in check-point %p day %d\n", a->n, b->n, cp, cp->day);
    exit(3);
  }
  if ( n1 && n2 )
  {
    return p;
  }
  int what;
  if ( n1 )
    what = b->n;
  else
    what = a->n;
  p = cp;
  for ( ; cp; cp = cp->cp )
  {
    n1 = bset_test(cp->check_point, what);
#ifdef TIME
    g_cp_find_ops++;
#endif
    if ( n1 )
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
  parent *cp = get_cp(p);
//  if ( !cp )
//    p->dump();
  if ( cp && cp->check_point )
  {
    auto n1 = bset_test(cp->check_point, a->n);
    auto n2 = bset_test(cp->check_point, b->n);
#ifdef TIME
    g_cp_find_ops += 2;
#endif
    if ( !n1 && !n2 )
    {
      printf("skip bug - cannot find both %d & %d in check-point %p day %d\n", a->n, b->n, cp, cp->day);
      exit(3);
    } else
    if ( n1 && n2 )
    {
      *up_to = cp;
    } else {
      int what;
      if ( n1 )
        what = b->n;
      else
        what = a->n;
      p = cp;
      for ( ; cp; cp = cp->cp )
      {
        n1 = bset_test(cp->check_point, what);
#ifdef TIME
        g_cp_find_ops++;
#endif
        if ( n1 )
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
  } else {
#ifdef TIME
    g_no_skips++;
#endif    
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
    swap(an, bn);
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
    CP_MAX = atoi(argv[1]);
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int m, q;
  cin>>g_n>>m>>q;
  if ( argc > 2 )
    CHAIN_MAX = atoi(argv[2]);
  setup();
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
  printf("skipped %ld not skipped %d max %d avg %f\n", g_skipped, g_no_skips, g_skipped_max, (double)g_skipped / q);
  printf("visited %ld max %d avg %f\n", g_cp_visited, g_cp_visited_max, (double)g_cp_visited / q);
  printf("visited back %ld max %d avg %f\n", g_visited_back, g_visited_back_max, (double)g_visited_back / q);
  printf("skipped cp %d, back skipped %d, find ops %ld\n", g_skipped_cp, g_back_skipped_cp, g_cp_find_ops);
  printf("check-points %d cloned %d total size %ld nodes, max size %ld, avg size %f\n", 
    g_cp_count, g_cp_cloned, g_cp_size, g_cp_size_max, (double)g_cp_size / g_cp_count);
#endif
}
