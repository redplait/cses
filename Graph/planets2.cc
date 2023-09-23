#include <stdlib.h>
#include <stdio.h>
#include <cstddef>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

/* Planets Queries II
 * Planet P with out-coming node T can be
 *  dead-end if no T or T points to P itself
 *  in cycle - then we need all planets in cycle and index of P
 *  just some route ending in previois 2 cases
*/

// size of cycle
#define KSIZE 10
// size of routes
#define RSIZE 500
// min size to cache route
#define MIN_CSIZE 2000

struct planet
{
  int n;
  planet *edge;
  vector<planet *> *big_cycle = nullptr;
  map<int, int> *my_routes = nullptr;
  // key in planet number, value is cycle index
  map<int, int> *in_cycle = nullptr;
  int big_index;
  // dfs data for cycles
  int k = 0; // size of cycle. for example if k = 3 then route[0] -> route[1] -> route[2] is this
  char color = 0; // 0 - not visited, 1 - processing, 2 - in cycle, 3 - processed
  bool visited() const
    { return !color; }
  // has incoming edges?
  int has_in = 0;
  // just route to end or cycle
  planet *end = nullptr;
  int route_size = 0;
  bool cycle_owner = false;
  bool in_cycle_owner = false;
  bool my_routes_owner = false;
  ~planet()
  {
    if ( big_cycle && cycle_owner )
    {
      delete big_cycle;
    }
    if ( in_cycle && in_cycle_owner )
      delete in_cycle;
    if ( my_routes && my_routes_owner )
      delete my_routes;
  }
  inline int dead_end() const
  { return !edge || (this == edge); }
};

typedef planet* pplanet;

void dump_planets(const vector<planet> &planets, bool dump_color = false)
{
  printf("\n");
  for ( int i = 0; i < planets.size(); i++ )
  {
    printf("%d k %d ", planets[i].n, planets[i].k);
    if ( planets[i].dead_end() )
    {
      printf("dead-end\n");
      continue;
    }
    if ( planets[i].route_size )
      printf("rsize %d ", planets[i].route_size);
    auto curr = planets[i].edge;
    for ( int j = 0; j < planets[i].route_size; ++j, curr = curr->edge )
      printf("%d ", curr->n);
    if ( planets[i].big_cycle )
    {
      printf("big_index %d size %ld", planets[i].big_index, planets[i].big_cycle->size());
      for ( auto c = planets[i].big_cycle->cbegin(); c != planets[i].big_cycle->cend(); ++c )
        printf(" %d", (*c)->n);
    } else if ( planets[i].in_cycle && planets[i].in_cycle_owner )
    {
      auto &m = *planets[i].in_cycle;
      printf("map ");
      for ( auto mi: m )
        printf("[%d %d] ", mi.first, mi.second); 
    } else if (planets[i].end)
      printf("end %d ", planets[i].end->n);
    else if ( planets[i].k )
      printf("%d ", planets[i].edge->n);
    if ( dump_color )
      printf("color %d", planets[i].color);
    printf("\n");
  } 
}

// Based on https://neerc.ifmo.ru/wiki/index.php?title=%D0%98%D1%81%D0%BF%D0%BE%D0%BB%D1%8C%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%BE%D0%B1%D1%85%D0%BE%D0%B4%D0%B0_%D0%B2_%D0%B3%D0%BB%D1%83%D0%B1%D0%B8%D0%BD%D1%83_%D0%B4%D0%BB%D1%8F_%D0%BF%D0%BE%D0%B8%D1%81%D0%BA%D0%B0_%D1%86%D0%B8%D0%BA%D0%BB%D0%B0
// complexity O(E)
void dfs(planet *curr, int N, const bool add)
{
  if ( curr->dead_end() )
  {
    curr->color = 4;
    return;
  }
#ifdef DEBUG
  printf("dfs %d color %d\n", curr->n, curr->color); fflush(stdout);
#endif
  vector<planet *> visited;
  visited.reserve(N);
  int csize = 1;
  bool has_cycle = false;
  planet *root = curr;
  while( curr )
  {
    visited.push_back(curr);
    csize++;
    if ( curr->dead_end() )
    {
      break;
    }
// printf("%d ", curr->n);
    if ( curr->edge == root )
    {
      has_cycle = true;
      break;
    }
    if ( curr->k )
    {
      // this means that path from root leads to some known cycle and we can safely mark all nodes to this as visited
#ifdef DEBUG
  printf("dfsk %d break at %d, k %d\n", root->n, curr->n, curr->k);
#endif
      visited.pop_back(); // remove thus node bcs it belongs to cycle
      int dist = 1;
      for ( auto e = visited.rbegin(); e != visited.rend(); ++dist, ++e )
      {
        (*e)->color = 3;
        (*e)->route_size = dist;
        (*e)->end = curr;
      }
      return;
    }
    if ( curr->color == 1 )
    {
      // this means that root leads to some unknown cycle not contained root itselt
      // but curr is part of this cycle
#ifdef DEBUG
  printf("dfs %d break at %d, color %d\n", root->n, curr->n, curr->color);
#endif
      for ( auto e: visited )
        e->color = 0;
      dfs(curr, N, false);
      // mark all planets not in cycle as visited
      for ( auto e: visited )
        e->color = 2;
      // remove all planets in cycle. we could use remove_if but it returns just past-the-end iterator but we then must have rbegin
      while(1)
      {
        auto e = visited.back();
        if ( e->k )
          visited.pop_back();
        else
          break; 
      }
#ifdef DEBUG
 for ( auto e: visited )
   printf("%d ", e->n);
  printf("\n");
#endif
      int dist = 1;
      for ( auto e = visited.rbegin(); e != visited.rend(); ++dist, ++e )
      {
#ifdef DEBUG
 printf("asgn rsize %d last %d to %d\n", dist, curr->n, (*e)->n);
#endif
        (*e)->route_size = dist;
        (*e)->end = curr;
      }
      return;
    }
    if ( curr->color )
    {
      // some already discovered route
#ifdef DEBUG
  printf("dfs %d break at %d, color %d\n", root->n, curr->n, curr->color);
#endif
      visited.pop_back(); // remove thus node bcs it belongs to cycle
      int dist = curr->route_size + 1;
#ifdef DEBUG
 printf("dist %d ", dist);
 for ( auto e: visited )
   printf("%d ", e->n);
  printf("\n");
#endif
      for ( auto e = visited.rbegin(); e != visited.rend(); ++dist, ++e )
      {
        if ( (*e)->route_size )
          break;
        (*e)->color = 4;
        (*e)->route_size = dist;
        (*e)->end = curr->end;
      }
      return;
    }
    curr->color = 1;
    curr = curr->edge;
  }
  if ( !has_cycle )
  {
    visited.pop_back(); // remove top node bcs it is end
    int dist = 1;
    for ( auto e = visited.rbegin(); e != visited.rend(); ++dist, ++e )
    {
      (*e)->color = 5;
      (*e)->route_size = dist;
      (*e)->end = curr;
    }
    return;
  }
  csize--;
#ifdef DEBUG
  printf("found cycle, root %d csize %d\n", root->n, csize);
#endif
  root->in_cycle = new map<int, int>;
  root->in_cycle_owner = true;
/*  if ( csize > KSIZE )
  {
    root->big_cycle = new vector<planet *>;
    root->cycle_owner = true;
    root->big_cycle->reserve(csize);
  } */
   int idx = 0;
   for ( auto e: visited )
   {
     e->color = 2;
     e->k = csize;
     e->in_cycle = root->in_cycle;
     root->in_cycle->insert({e->n, idx});
     /* if ( csize > KSIZE )
     {
       root->big_cycle->push_back(e);
       e->big_cycle = root->big_cycle;
       e->big_index = idx;
     } */
     idx++;
   }
}

int query(int a, int b, const vector<planet> &db)
{
  const planet *curr = &db[a - 1];
  int res = 0;
next:
  if ( curr->n == b )
    return res;
  if ( curr->dead_end() )
    return -1;
  if ( curr->k )
  {
    auto in_it = curr->in_cycle->find(b);
    if ( in_it == curr->in_cycle->end() )
      return -1;
    auto curr_it = curr->in_cycle->find(curr->n);
#ifdef DEBUG
  printf("in_cycle %d pos %d b %d pos %d k %d\n", curr->n, curr_it->second, b, in_it->second, curr->k);
#endif
    if ( in_it->second > curr_it->second )
      return res + in_it->second - curr_it->second;
    res += curr->k - curr_it->second;
    return res + in_it->second;
  }
  if ( curr->my_routes )
  {
    auto in_it = curr->my_routes->find(b);
    if ( in_it == curr->my_routes->end() )
    {
      res += curr->route_size;
      curr = curr->end;
      goto next;
    }
    auto curr_it = curr->my_routes->find(curr->n);
    if ( curr_it->second > in_it->second )
      return -1;
#ifdef DEBUG
  printf("my_routes %d pos %d b %d pos %d route_size %d\n", curr->n, curr_it->second, b, in_it->second, curr->route_size);      
#endif
    return res + in_it->second - curr_it->second;
  }
  curr = curr->edge;
  res++;
  goto next;
}

inline void printTime(const char *pfx)
{
#ifdef DEBUG
  printf("%s: %f\n", pfx, (float)clock()/CLOCKS_PER_SEC);
#endif
}

void fill_my_routes(planet *p)
{
  auto curr = p->edge;
  if ( curr->my_routes )
    return;
  if ( curr->route_size < RSIZE )
    return;
  p->my_routes = new map<int, int>;
  p->my_routes_owner = true;
  p->my_routes->insert({p->n, 0});
  for ( int j = 0; j < p->route_size; ++j, curr = curr->edge )
  {
    curr->my_routes = p->my_routes;
    curr->my_routes_owner = false;
    p->my_routes->insert({curr->n, 1 + j});
  }
}

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, q;
  cin>>n; cin>>q;
  vector<planet> planets(n);
  for ( int i = 0; i < n; ++i )
  {
    int t;
    cin>>t;
    planets[i].n = i + 1;
    planets[i].k = 0;
    planets[i].edge = &planets[t-1];
    if ( !planets[i].dead_end() )
      planets[t-1].has_in++;
  }
printTime("start");
  // count cycles
  for ( int i = 0; i < n; ++i )
  {
    auto &curr = planets[i];
    // planet has dead-end?
    if ( curr.dead_end() )
      continue;
    if ( curr.has_in )
      continue;
    if ( !curr.color && !curr.k )
    {
      dfs(&curr, n, true);
      // dump_planets(planets, true);
    }
  }
  printTime("cycles");
#ifdef DEBUG
  dump_planets(planets);
#endif
  // secons pass to collect remaibed routes
  for ( int i = 0; i < n; ++i )
  {
    auto &curr = planets[i];
    // planet has dead-end?
    if ( curr.dead_end() )
      continue;
    if ( !curr.color && !curr.k )
      dfs(&curr, n, false);
  }
 // dump_planets(planets, false);
  printTime("before routes");
  // fill weights
  typedef pair<planet *, int64_t> pw;
  vector<pw> weights;
  for ( int i = 0; i < n; ++i )
  {
    auto &curr = planets[i];
    // planet has dead-end?
    if ( curr.dead_end() )
      continue;
    if ( curr.k )
      continue;
    if ( !curr.has_in )
      weights.push_back({&curr, curr.route_size});
    else
      weights.push_back({&curr, curr.route_size * curr.has_in});
  }
  if ( weights.size() )
  {
    // lets try to find hottest routes for caching
    sort(weights.begin(), weights.end(), [](const pw &a, const pw &b) { return a.second > b.second;});
    // for ( auto &w: weights )
    //  printf("%d %ld in %d\n", w.first->n, w.second, w.first->has_in);
    if ( !weights.begin()->first->has_in )
      for ( auto &w: weights )
      {
        if ( w.first->has_in )
          break;
        if ( w.first->my_routes )
          continue;
        if ( w.second < 2000 )
          break;
        fill_my_routes(w.first);
      }
    else
      for ( auto &w: weights )
      {
        if ( w.second < MIN_CSIZE )
          break;
        if ( w.first->my_routes )
          continue;
        fill_my_routes(w.first);
      }
  }
  printTime("routes");
#ifdef DEBUG
  dump_planets(planets);
  fflush(stdout);
#endif
  // process queries
  for ( int j = 0; j < q; ++j )
  {
    int a, b;
    cin>>a>>b;
#ifdef DEBUG
printf("%d q %d a %d b %d: ", j, q, a, b);
#endif
    auto &bb = planets[b-1];
    if ( a == b )
      printf("0\n");
    else if ( !bb.has_in )
      printf("-1\n");
    else
      printf("%d\n", query(a, b, planets));
  }
  printTime("results");
}