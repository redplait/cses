#include <stdlib.h>
#include <stdio.h>
#include <cstddef>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

/* Planets Queries I
 * Planet P with out-coming node T can be
 *  dead-end if no T or T points to P itself
 *  in cycle - then we need all planets in cycle and index of P
 *  just some route ending in previois 2 cases
*/

// size of cycle
#define KSIZE 10

struct planet
{
  int n;
  planet *edge;
  vector<planet *> *big_cycle = nullptr;
  int big_index;
  // dfs data for cycles
  int k = 0; // size of cycle. for example if k = 3 then route[0] -> route[1] -> route[2] is this
  char color = 0; // 0 - not visited, 1 - processing, 2 - in cycle, 3 - processed
  bool visited() const
    { return !color; }
  // has incoming edges?
  bool has_in = false;
  // just route to end or cycle
  planet *end = nullptr;
  int route_size = 0;
  bool cycle_owner = false;
  ~planet()
  {
    if ( big_cycle && cycle_owner )
    {
      delete big_cycle;
    }
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
  if ( csize > KSIZE )
  {
    root->big_cycle = new vector<planet *>;
    root->cycle_owner = true;
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
    if ( !curr->big_cycle )
    {
      curr = curr->edge;
      k--;
      goto next;
    }
    // we have planet in some big cycle
    int rem = curr->k - curr->big_index;
    if ( k < rem )
        return curr->big_cycle->at(curr->big_index + k)->n;
    k -= rem;
    return curr->big_cycle->at(k)->n;
  }
  if ( k < curr->route_size )
  {
    k--;
    // move to next planet
    curr = curr->edge;
    goto next;
  }
  k -= curr->route_size;
  curr = curr->end;
// printf("query k %d at %d\n", k, curr->n);
  goto next;
}

inline void printTime(const char *pfx)
{
#ifdef DEBUG
  printf("%s: %f\n", pfx, (float)clock()/CLOCKS_PER_SEC);
#endif
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
      planets[t-1].has_in = true;
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
   
  printTime("routes");
#ifdef DEBUG
  dump_planets(planets);
  fflush(stdout);
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