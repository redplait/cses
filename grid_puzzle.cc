#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

// solution for Grid Puzzle I
// nothing special - just place all columns to priority queue sorted for values in each column
// then for each row pop some column, dec it`s value and push it back if non-zero
// if we was unable to place N elements for current row or priority queue is empty then solution don`t exists
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")
using namespace std;
typedef pair<int,int> pi;

struct customLess {
 bool operator()(const pi &a, const pi &b) { return a.second < b.second;}
};

struct sq
{
  vector<string> res;
  vector<pi> rows;
  priority_queue< pi, vector<pi>, customLess > cols;
  int N;
  sq(int n): res(n), N(n)
  {
    for ( int i = 0; i < N; ++i ) res[i].resize(N, '.');
  }
  void add_row(int r, int v)
  {
    if (!v) return;
    rows.push_back({r, v});
  }
  void add_col(int c, int v)
  {
    if (!v) return;
    cols.push({c,v});
  }
  inline int check(int r, int c)
  {
    return 'X' == res[r].at(c);
  }
  void dump()
  {
    for ( auto &s: res ) printf("%s\n", s.c_str());
  }
  int calc()
  {
    sort(rows.begin(), rows.end(), [](const pi &a, const pi &b) { return a.second < b.second; });
    for ( pi &r: rows )
    {
      if ( cols.empty() ) return 0;
      vector<pi> skipped;
      int placed = 0;
      for ( int i = 0; i < r.second && !cols.empty(); i++ )
      {
        auto t = cols.top();
#ifdef DEBUG
 printf("%d %d top %d %d\n", r.first, r.second, t.first, t.second);
#endif
        if ( check(r.first, t.first) ) { skipped.push_back(t); cols.pop(); i--; continue; }
        res[r.first].at(t.first) = 'X'; placed++;
        if ( t.second > 1 )
        {
          pi p{t.first, t.second-1};
          cols.pop(); cols.push(p);
        } else
          cols.pop();
      }
      if ( placed != r.second ) return 0;
      for ( auto &s: skipped ) cols.push(s);
    }
    return cols.empty();
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
  if ( !g.calc() ) puts("-1");
  else g.dump();
}