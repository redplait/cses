#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>

// wrong solution for Grid Puzzle II
// seems that stupid greedy algo don`t work even if first try to fill squares on cross with rows & cols having 1
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")
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

struct item
{
  int val, row, col, pr;
  bool used = false;
};

struct customLess {
 bool operator()(const item *a, const item *b) { return a->pr == b->pr ? a->val < b->val : b->pr < a->pr;}
};

struct sq
{
  vector<string> res;
  vector<pair<int, int> > rows, cols;
  vector<vector<item *> > rdata; // items arranged by rows
  int N;
  size_t res_v, rv;
  sq(int n): res(n), rows(n), cols(n), rdata(n), N(n), res_v(0)
  {
    for ( int i = 0; i < N; ++i ) res[i].resize(N, '.');
  }
  void reset()
  {
    for ( auto &i: rows ) i.second = i.first;
    for ( auto &i: cols ) i.second = i.first;
    for ( int i = 0; i < N; ++i ) fill(res[i].begin(), res[i].end(), '.');
  }
  inline void add_row(int r, int v)
  {
    rows[r].first = rows[r].second = v;
  }
  inline void add_col(int c, int v)
  {
    cols[c].first = cols[c].second = v;
  }
  int acc(const vector<pair<int, int> > &v)
  {
    int res = 0;
    for ( auto &p: v ) res += p.first;
    return res;
  }
  int quick_check()
  {
    rv = acc(rows);
    size_t cv = acc(cols);
#ifdef DEBUG
 printf("rv %ld vc %ld\n", rv, cv);
#endif
    return rv == cv ? 1 : 0;
  }
  void add_item(int r, int c, int v)
  {
    if (!rows[r].first) return;
    if (!cols[c].first) { rdata[r].push_back(nullptr); return; }
    item *i = new item{v, r, c};
    rdata[r].push_back(i);
  }
  inline int check(int r, int c)
  {
    if (!rows[r].second) return 0;
    if (!cols[c].second) return 0;
    return 1;
  }
  void dump()
  {
    printf("%ld\n", res_v);
    for ( auto &s: res ) printf("%s\n", s.c_str());
  }
  void dump_bad()
  {
    printf("rows:");
    for ( auto &i: rows ) printf(" %d", i.second);
    printf("\ncols:");
    for ( auto &i: cols ) printf(" %d", i.second);
    printf("\n");
    dump();
  }
  void mark(item *i, size_t &inserted)
  {
    rows[i->row].second--; cols[i->col].second--; res_v += i->val; i->used = true;
    inserted++;
    res[i->row].at(i->col) = 'X';
  }
  void fill_q(priority_queue<item *, vector<item *>, customLess> &q, size_t &inserted)
  {
    while( !q.empty() && inserted != rv )
    {
      auto i = q.top();
#ifdef DEBUG
 printf("top %d row %d col %d pr %d\n", i->val, i->row, i->col, i->pr);
#endif
      if ( check(i->row, i->col) )
      {
        mark(i, inserted);
      } else {
        // skipped.push_back(i);   
#ifdef DEBUG
    printf("skip, row %d, col %d\n", rows[i->row], cols[i->col]);
#endif
      }
        q.pop();
    }
  }
  // fill squares on crossing row & cols with degree 1
  int p1(size_t &inserted)
  {
    int cont = 0;
    do {
      cont = 0;
    // lets fill rows with degree 1
    vector<int> r1;
    for ( int i = 0; i < N; i++ )
      if ( rows[i].second == 1 ) { cont++; r1.push_back(i); }
    for ( auto i: r1 )
    {
      // find max value on this row
      item *found = nullptr;
      for ( auto v: rdata[i] )
      {
        if ( !v || v->used ) continue;
        if ( !cols[v->col].second ) continue;
        if ( !found ) found = v;
        else if ( v->val > found->val ) found = v;
      }
      if ( !found ) {
        printf("cannot resolve r1 for %d\n", i);
        return 0;
      }
      mark(found, inserted);
    }
    // and the same for cols with degree 1
    r1.clear();
    for ( int i = 0; i < N; i++ )
      if ( cols[i].second == 1 ) { cont++; r1.push_back(i); }
    for ( auto i: r1 )
    {
#ifdef DEBUG
printf("process col1 %d\n", i);
#endif
      // find max value for this column
      item *found = nullptr;
      for ( int j = 0; j < N; j++ )
      {
        if ( !rows[j].second ) continue;
        item *v = rdata[j].at(i);
        if ( !v || v->used ) continue;
        if ( !found ) found = v;
        else if ( v->val > found->val ) found = v;
      }
      if ( !found ) {
        printf("cannot resolve c1 for %d\n", i);
        return 0;
      }
      mark(found, inserted);
    }
#ifdef DEBUG
    if ( cont )
      dump_bad();
#endif
    } while(cont);
    return 1;
  }
  int calc()
  {
    size_t inserted = 0;
    if ( !p1(inserted) ) return 0;
    int cont;
    do {
      cont = 0;
      // priority_queue<item *, vector<item *>, customLess> q;
      vector<item *> q;
      for ( auto &r: rdata )
        for ( auto e: r ) if ( e && !e->used ) {
          e->pr = rows[e->row].second * cols[e->col].second;
          if ( e->pr ) q.push_back(e);
        }
      if ( q.empty() ) return 0;
      sort(q.begin(), q.end(), [](item *a, item *b) { return a->pr == b->pr ? a->val > b->val : a->pr < b->pr; });
#ifdef DEBUG
 printf("mark %d %d val %d pr %d\n", q[0]->row, q[0]->col, q[0]->val, q[0]->pr);
#endif
      mark(q[0], inserted);
      cont++;
      cont += p1(inserted);
    } while(cont);
#ifdef DEBUG
 printf("inserted %ld\n", inserted);
#endif
    if ( inserted == rv ) return 1;
    dump_bad();
    return 0;
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n;
  cin>>n;
  sq g(n);
  // read rows
  for ( int i = 0; i < n; ++i )
  {
    int v;
    cin>>v;
    g.add_row(i, v);
  }
  // read columns
  for ( int i = 0; i < n; ++i )
  {
    int v;
    cin>>v;
    g.add_col(i, v);
  }
  if ( !g.quick_check() ) puts("-1");
  else {
    for ( int r = 0; r < n; r++ )
      for ( int c = 0; c < n; c++ )
      {
        int v;
        cin>>v;
        g.add_item(r, c, v);
      }
printTime("read");
    if ( !g.calc() ) puts("-1");
    else g.dump();
printTime("calc");
  }
}