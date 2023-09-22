#include <stdlib.h>
#include <stdio.h>
#include <cstddef>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

/* Planets Queries I
 * this version uses greedy algo to make cache of all routes (and so they are not optimal)
 * It gives right routes but too slow - see https://cses.fi/problemset/result/7159298/
 */

#define DFS
// size of cycle
#define KSIZE 10
// size of cache
#define CSIZE 200

struct planet
{
  int n;
  vector<planet *> route;
  vector<planet *> *big_cycle = nullptr;
  int big_index;
  // from dfs for cycles
  int k; // size of cycle. for example if k = 3 then route[0] -> route[1] -> route[2] is this
  char color = 0; // 0 - not visited, 1 - gray, 2 - black
  char cvisited = 0;
  // cache for long routes
  const planet **cache = nullptr;
  int cache_size;
  bool cache_owner = false;
  ~planet()
  {
    if ( cache && cache_owner )
     delete[] cache;
  }
  inline int dead_end() const
  { return this == route[0]; }
};

typedef planet* pplanet;

// Based on https://neerc.ifmo.ru/wiki/index.php?title=%D0%98%D1%81%D0%BF%D0%BE%D0%BB%D1%8C%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%BE%D0%B1%D1%85%D0%BE%D0%B4%D0%B0_%D0%B2_%D0%B3%D0%BB%D1%83%D0%B1%D0%B8%D0%BD%D1%83_%D0%B4%D0%BB%D1%8F_%D0%BF%D0%BE%D0%B8%D1%81%D0%BA%D0%B0_%D1%86%D0%B8%D0%BA%D0%BB%D0%B0
// complexity O(E)
void dfs(planet *curr, int N)
{
  if ( curr->dead_end() )
  {
    curr->cvisited = 4;
    return;
  }
#ifdef DEBUG
  printf("dfs %d cvis %d\n", curr->n, curr->cvisited);
#endif
  vector<planet *> visited;
  visited.reserve(N);
  int csize = 1;
  bool has_cycle = false;
  planet *root = curr;
  while( !curr->route.empty() )
  {
    if ( curr->dead_end() )
      break;
// printf("%d ", curr->n);
    if ( curr->route[0] == root )
    {
      visited.push_back(curr);
      has_cycle = true;
      break;
    }
    if ( curr->route[0]->k )
    {
      // this means that path from root leads to some known cycle and we can safely mark all nodes to this as cvisited
#ifdef DEBUG
  printf("dfsk break at %d, k %d\n", curr->n, curr->route[0]->k);
#endif
      for ( auto e: visited )
      {
        e->color = 0;
        e->cvisited = 1;
      }
      return;
    }
    if ( curr->route[0]->color )
    {
      // this means that root leads to some unknown cycle not contained root itselt
      // but curr->route[0] is part of this cycle
#ifdef DEBUG
  printf("dfs break at %d, color %d\n", curr->n, curr->route[0]->color);
#endif
      for ( auto e: visited )
        e->color = 0;
      dfs(curr->route[0], N);
      // mark all planets till first having k as visited
      for ( auto e: visited )
      {
        if ( e->k )
          break;
        e->cvisited = 2;
      }
      return;
    }
    curr->color = 1;
    visited.push_back(curr);
    csize++;
    curr = curr->route[0];
  }
  if ( !has_cycle )
  {
     for ( auto e: visited )
     {
        e->color = 0;
        e->cvisited = 3;
     }
     return;
  }
   if ( csize > KSIZE )
   {
     root->big_cycle = new vector<planet *>; // yep, there is root of mem leaks. should store alloced vector to somewhere to delete later
     root->big_cycle->reserve(csize);
   }
   int idx = 0;
   for ( auto e: visited )
   {
     e->color = 2;
     e->k = csize;
     if ( csize > KSIZE )
     {
       root->big_cycle->push_back(e);
       e->big_cycle = root->big_cycle;
       e->big_index = idx++;
     }
   }
}

int query(int p, int k, const vector<planet> &db)
{
  const planet *curr = &db[p - 1];
next:
  if ( !k )
    return curr->n;
  if ( curr->dead_end() )
    return curr->n;
  if ( curr->k )
  {
    k = k % curr->k;
    if ( !k )
      return curr->n;
    if ( k < curr->route.size() )
      return curr->route[k-1]->n;
    if ( curr->big_cycle )
    {
      int rem = curr->k - curr->big_index;
      if ( k < rem )
         return curr->big_cycle->at(curr->big_index + k)->n;
      k -= rem;
      return curr->big_cycle->at(k)->n;
    } else {
      // move to next element
      curr = curr->route[0];
      k--;
      goto next;
    }
  }
  k--;
  if ( curr->cache )
  {
    if ( k < curr->cache_size )
      return curr->cache[k]->n;
    auto last = curr->cache[curr->cache_size - 1];
    if ( last->k )
      return query(last->n, k + 1 - curr->cache_size, db);
    return last->n;
  } else {
    if ( k < curr->route.size() )
      return curr->route[k]->n;
    auto last = curr->route.back();
    if ( last->k )
      return query(last->n, k + 1 - curr->route.size(), db);
    return last->n;
  }
}

inline void printTime(const char *pfx)
{
// #ifdef DEBUG
  printf("%s: %f\n", pfx, (float)clock()/CLOCKS_PER_SEC);
// #endif
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
    planets[i].route.push_back(&planets[t-1]);
  }
printTime("start");
  // count cycles
  for ( int i = 0; i < n; ++i )
  {
    auto &curr = planets[i];
    if ( curr.route.empty() )
      continue; // wtf - no teleport on this planet
    // planet has dead-end?
    if ( curr.dead_end() )
      continue;
#ifdef DFS
    if ( !curr.color && !curr.cvisited && !curr.k )
      dfs(&curr, n);
#else
    planet *next = planets[i].route[0];
    for ( int j = 1; j < n; j++ )
    {
      if ( next->dead_end() )
        break;
      next = next->route[0];
      if ( next == &planets[i] )
      {
        planets[i].k = j + 1;
        break;
      }
    }
#endif
  }
printTime("cycles");
  // build routes
  for ( int i = 0; i < n; ++i )
  {
    auto &curr = planets[i];
    if ( curr.route.empty() )
      continue; // wtf - no teleport on this planet
    if ( curr.cache )
      continue; // already filled
    // planet has dead-end?
    if ( curr.dead_end() )
      continue;
    planet *next = curr.route[0];
    if ( next->k )
      continue;
    for ( int j = 1; j < n; j++ )
    {
      if ( next->dead_end() )
        break;
      next = next->route[0];
      curr.route.push_back(next);
      if ( next == &curr )
        break;
      if ( next->k )
        break;
    }
    if ( !curr.cache && !curr.k && curr.route.size() > CSIZE && !curr.route[0]->cache )
    {
#ifdef DEBUG
      printf("caching need for %d\n", curr.n);
#endif
      // make cache for current planet
      auto rsize = curr.route.size();
      pplanet *my = new pplanet[rsize];
      copy(curr.route.begin(), curr.route.end(), my);
      curr.cache = (const planet **)my;
      curr.cache_owner = true;
      curr.cache_size = rsize;
      // copy this route to all planets in it
      for ( int r = 0; r < curr.route.size(); ++r )
      {
        planet *pr = curr.route[r];
        if ( pr->cache ) // all next planets in this route already was cached
          break;
        pr->cache = (const planet **)&my[r+1];
        pr->cache_size = curr.route.size() - r - 1;
        if ( pr->cache_size < CSIZE )
          break;
      }
    }
  }
printTime("routes");
#ifdef DEBUG
  // dump planets
  printf("\n");
  for ( int i = 0; i < n; i++ )
  {
    printf("%d k %d ", planets[i].n, planets[i].k);
    if ( planets[i].dead_end() )
    {
      printf("dead-end\n");
      continue;
    }
    for ( planet *p: planets[i].route )
      printf("%d ", p->n);
    if ( planets[i].big_cycle )
    {
      printf("big_index %d size %ld", planets[i].big_index, planets[i].big_cycle->size());
      for ( auto c = planets[i].big_cycle->cbegin(); c != planets[i].big_cycle->cend(); ++c )
        printf(" %d", (*c)->n);
    } else if ( planets[i].cache )
    {
      if ( planets[i].cache_owner )
        printf("cache_owner");
      else {
        printf("cached ");
        auto ca = *planets[i].cache;
        for ( int j = 0; j < planets[i].cache_size; ++j )
          printf("%d ", ca[j].n);
      }
    }
    printf("\n");
  }
#endif
  // process queries
  for ( int j = 0; j < q; ++j )
  {
    int x, k;
    cin>>x>>k;
#ifdef DEBUG
printf("%d q %d x %d K %d: ", j, q, x, k);
#endif
    printf("%d\n", query(x, k, planets));
  }
printTime("results");
}