#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>

// Maximum Building I
// strange but version with priority_queue much slower on samples 1000 x 1000

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

struct sq
{
  int x, y;
  int ar = 0, // count of available squares at right
      ab = 0; // count of available squares below
  bool empty = true; // false if has tree
  inline int rb() const { return ar * ab; }
  void set(char c, int y_, int x_)
  {
    x = x_; y = y_;
    if ( c == '*' ) empty = false;
  }
} __attribute__ ((aligned (4)));

struct customCmp {
 bool operator()(const sq *a, const sq *b) { return a->rb() > b->rb(); }
};

struct mb
{
  vector< vector<sq> > M;
  int n; // num of rows
  int c; // num of cols
  vector<sq *> Q;
  mb(int N, int m): M(N), n(N), c(m)
  {
    for ( int i = 0; i < N; i++ ) M[i].resize(c);
  }
  void add(char c, int y, int x)
  {
    M[y][x].set(c, y, x);
  }
  int car(int y, int x)
  {
    if ( x >= c ) return 0;
    if ( !M[y][x].empty ) return 0;
    if ( !M[y][x].ar ) M[y][x].ar = 1 + car(y, x+1);
    return M[y][x].ar;
  }
  int cab(int y, int x)
  {
    if ( y >= n ) return 0;
    if ( !M[y][x].empty ) return 0;
    if ( !M[y][x].ab ) M[y][x].ab = 1 + cab(y+1, x);
    return M[y][x].ab;
  }
  int prepare()
  {
    for ( int i = 0; i < n; ++i )
      for ( int j = 0; j < c; ++j )
      {
        if ( !M[i][j].empty ) continue;
        if ( !M[i][j].ar ) M[i][j].ar = 1 + car(i, j+1);
        if ( !M[i][j].ab ) M[i][j].ab = 1 + cab(i+1, j);
        if ( M[i][j].rb() > 1 ) Q.push_back(&M[i][j]);
      }
    sort(Q.begin(), Q.end(), customCmp());
    return !Q.empty();
  }
  int calc()
  {
    int max_s = 1;
    for( auto curr: Q )
    {
      int c_s = curr->rb();
      if ( c_s <= max_s ) break; // skip all remained cells bcs their potential square lesser than currently found
#ifdef DEBUG
 printf("%d %d ar %d ab %d max_s %d\n", curr->y, curr->x, curr->ar, curr->ab, max_s);
#endif
      int max_line = max(curr->ar, curr->ab);
      if ( max_line > max_s ) max_s = max_line;
      int down = curr->ab;
      for ( int i = 1; i < curr->ar; ++i )
      {
        auto right = &M[curr->y][curr->x + i];
        int check = 0;
        if ( right->ab < down )
        { down = right->ab; check = 1; }
        c_s = down * (i+1);
        if ( c_s > max_s ) max_s = c_s;
        // this is main optmization trick - if current ab lesser previous we need to check that square of all right cells
        // still can lead to maximum
        if ( check && down * right->ar <= max_s ) break;
      }
    }
    return max_s;
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  mb g(n, m);
  // read map
  for ( int i = 0; i < n; ++i )
  {
    string s;
    cin>>s;
    for ( int j = 0; j < m; ++j )
      g.add(s[j], i, j);
  }
  // calc max available distances for each square
  if ( !g.prepare() ) {
    puts("0");
    return 0;
  }
printTime("prepare");
  printf("%d\n", g.calc());
printTime("calc");
}