#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

// Grid Paths: https://cses.fi/problemset/task/1078
// naive solution with couple of rows
// sure it won`t able to process square with N = 1000000
using namespace std;
#define mod 1000000007

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

struct pair_hash
{
    size_t operator()(const pair<int, int>& s) const
    {
        return std::hash<int>{}(s.first) ^ (std::hash<int>{}(s.second) << 1);
    }
};

int g_dump_rem = 1;

struct sq
{
  vector<int> v;
  unordered_map<pair<int, int>, int, pair_hash> traps;
  int N;
  int *curr, *prev;
  sq(int n):
   v(n * 2), N(n)
  {
    fill(v.begin(), v.end(), 0);
    curr = &v[0]; prev = curr + N;
  }
  void upd_trap(int y, int x, int v)
  {
    pair<int, int> p{y-1,x-1};
    traps[p] = v;
  }
  void put_trap(int y, int x)
  {
    pair<int, int> p{y-1,x-1};
    traps.emplace(p, 0);
#ifdef DEBUG
  printf("%d %d: %d %d\n", y, x, numberOfPaths(y, x), numberOfPaths(N - y + 1, N - x + 1));
#endif
  }
  void dump_row(int *r)
  {
    for ( int i = 0; i < N; i++ ) printf("%d ", r[i]);
    printf("\n");
  }
  void dump_rem()
  {
    printf("remained traps:\n");
    for ( auto &ti: traps )
    {
      printf("%d %d - %d\n", 1 + ti.first.first, 1 + ti.first.second, ti.second);
    }
  }
  // row m and column n
  uint64_t numberOfPaths(int m, int n)
  {
    pair p{ m, n };
    // We have to calculate m+n-2 C n-1 here
    // which will be (m+n-2)! / (n-1)! (m-1)!
    uint64_t path = 1;
    for (int i = n; i < (m + n - 1); i++) {
        path *= i;
        path /= (i - n + 1);
        path = path % mod;
    }
    return path;
  }
  inline int is_trap(int y, int x)
  {
    pair p{ y - 1, x - 1 };
    auto ti = traps.find(p);
    return ti != traps.end();
  }
  void prepare()
  {
    curr[0] = 1;
    // make in curr paths from 1 - at left top corner to right
    for ( int i = 1; i < N; i++ )
    {
      if ( is_trap(1, i+1) ) break;
      curr[i] = 1;
    }
#ifdef DEBUG
 dump_row(curr);
#endif
    swap(curr, prev);
  }
  int calc()
  {
    // couple of nested cycles
    for ( int y = 2; y <= N; ++y )
    {
      fill(curr, curr + N, 0);
      if ( is_trap(y, 1) ) curr[0] = 0;
      else curr[0] = prev[0];
      for ( int x = 1; x < N; ++x )
      {
        if ( is_trap(y, x+1) )
        {
          upd_trap(y, x+1, (prev[x] + curr[x-1]) % mod);
          continue;
        }
        curr[x] = (prev[x] + curr[x-1]) % mod;
      }
#ifdef DEBUG
 dump_row(curr);
#endif
      swap(curr, prev);
    }
    // return right bottom conrner
    return prev[N-1];
  }
};

int main(int argc, char **argv)
{
#ifdef TEST
  if ( argc != 2 )
  {
    printf("in test mode you need peek file in cmd line\n");
    exit(6);
  }
#endif
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  sq g(n);
  for ( int i = 0; i < m; ++i )
  {
    int a,b;
    cin>>a>>b;
    g.put_trap(a,b);
  }
  g.prepare();
  printf("%d\n", g.calc());
printTime("res");
  if ( g_dump_rem )
    g.dump_rem();
}