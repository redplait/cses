#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

// Grid Paths: https://cses.fi/problemset/task/1078
using namespace std;
#define mod 1000000007
#define IT int64_t

// ripped from https://www.geeksforgeeks.org/compute-ncrp-using-fermat-little-theorem/
// idea https://medium.com/@sudsjaiswal/day-5-combinatorics-and-modulo-by-prime-numbers-b38ed3935d43
const int mxn = 2e6 + 5;
IT fact[mxn + 1];
IT pw(IT a, IT b){
    IT res = 1;
    for(;b;b >>= 1){
        if(b & 1){
            res = (res * a) % mod;
        }
        a = (a * a) % mod;
    }
    return(res);
}
IT choose(int k, int n){
    IT dem = (fact[k] * fact[n - k]) % mod;
    dem = pw(dem, mod - 2);
    return((fact[n] * dem) % mod);
}

IT xGCD(IT a, IT b, IT &x, IT &y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }

    IT x1, y1, gcd = xGCD(b, a % b, x1, y1);
    x = y1;
    y = x1 - (IT)((a / b) * y1 % mod);
    if ( y < 0 ) y += mod;
    return gcd;
}
IT inverse(IT a) {
    IT x, y;
    xGCD(a, mod, x, y);
    return x;
}

IT numberOfPaths(int m, int n)
{
  return choose(m - 1, m + n - 2);
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

struct pair_hash
{
    size_t operator()(const pair<int, int>& s) const
    {
        return std::hash<int>{}(s.first) ^ (std::hash<int>{}(s.second) << 1);
    }
};

int g_dump_rem = 1;

struct trap
{
  trap(int x_, int y_): x(x_), y(y_) {}
  ~trap() { if (g ) delete g; }
  IT from_s, to_t;
  int x, y;
  trap *chain = nullptr; // 6
  pair<trap *, int> *g = nullptr; // 5
  char del = 0; // 4
};

struct sq
{
  vector<trap> traps;
  vector<trap *> sorted_x, sorted_y;
  // unordered_map<pair<int, int>, int, pair_hash> traps;
  int N;
  sq(int n):
   N(n)
  {
  }
  void put_trap(int y, int x)
  {
    trap n{ x, y }; 
    traps.push_back(n);
#ifdef DEBUG
  printf("%d %d: %s %s\n", y, x, numberOfPaths(y, x).get_str().c_str(), numberOfPaths(N - y + 1, N - x + 1).get_str().c_str());
#endif
  }
  void dump_rem()
  {
    printf("remained traps:\n");
    for ( auto t: sorted_y )
    {
      printf("%d %d - %c %d\n", t->y, t->x, '%', t->from_s);
    }
  }
  int rm_odd_y()
  {
    if ( sorted_x.size() < 2) return 0;
    if ( sorted_x[0]->x != 1 ) return 0;
    int res = 0;
    int y_min = sorted_x[0]->y;
    size_t last = sorted_x.size();
    for ( size_t i = 1; i < last && y_min < N; ++i )
    {
      if ( sorted_x[i]->x == sorted_x[i-1]->x )
      {
        if ( sorted_x[i]->y > y_min )
        {
          res++;
          sorted_x[i]->del = 1;
#ifdef DEBUG
 printf("mark_x %d %d y_min %d\n", sorted_x[i]->y, sorted_x[i]->x, y_min);
#endif
        }
        continue;
      }
      if ( sorted_x[i]->x != sorted_x[i-1]->x + 1 ) break;
      if ( sorted_x[i]->y >= y_min || sorted_x[i]->y + 1 == y_min )
      { y_min = sorted_x[i]->y; continue; }
      if ( i != last -1 && sorted_x[i+1]->x != sorted_x[i]->x ) break;
      for ( size_t j = i + 1; j < last; ++j )
      {
        if ( sorted_x[i]->x != sorted_x[j]->x ) return res;
        if ( sorted_x[j]->y >= y_min || sorted_x[j]->y + 1 == y_min )
        { y_min = sorted_x[j]->y; i = j; goto next; }
      }
      break;
  next: ;
    }
    return res;
  }
  int rm_odd_x()
  {
    if ( sorted_y.size() < 2) return 0;
    if ( sorted_y[0]->y != 1 ) return 0;
    int res = 0;
    int x_min = sorted_y[0]->x;
    size_t last = sorted_y.size();
    for ( size_t i = 1; i < last && x_min < N; ++i )
    {
      if ( sorted_y[i]->y == sorted_y[i-1]->y )
      {
        if ( sorted_y[i]->x > x_min )
        {
           res++;
           sorted_y[i]->del = 1;
#ifdef DEBUG
 printf("mark_y %d %d x_min %d\n", sorted_y[i]->y, sorted_y[i]->x, x_min);
#endif
        }
        continue;
      }
      if ( sorted_y[i]->y != sorted_y[i-1]->y + 1 ) break;
      if ( sorted_y[i]->x >= x_min || sorted_y[i]->x + 1 == x_min )
      { x_min = sorted_y[i]->x; continue; }
// printf("odd_x2 y %d x %d next %d\n", sorted_y[i]->y, sorted_y[i]->x, sorted_y[i+1]->y);
      if ( i != last -1 && sorted_y[i+1]->y != sorted_y[i]->y ) break;
      for ( size_t j = i + 1; j < last; ++j )
      {
        if ( sorted_y[i]->y != sorted_y[j]->y ) return res;
        if ( sorted_y[j]->x >= x_min || sorted_y[j]->x + 1 == x_min )
        { x_min = sorted_y[j]->x; i = j;
// printf("found new x_min %d\n", x_min); 
          goto next; }
      }
      break;
  next: ;
    }
    return res;
  }
  int rm_r(int from, int x, char v)
  {
    int res = 0;
    for ( size_t i = from + 1; i < sorted_y.size(); ++i )
    {
      if ( sorted_y[i]->y != sorted_y[from]->y ) break;
      if ( sorted_y[i]->x > x ) break;
      sorted_y[i]->del = v; res++;
    }
    return res;
  }
  int rm_r2(int from, int x, char v)
  {
    int res = 0;
    auto fr = sorted_y[from]->g;
    for ( size_t i = from + 1; i < sorted_y.size(); ++i )
    {
      if ( sorted_y[i]->y != sorted_y[from]->y ) break;
      if ( sorted_y[i]->x > x ) break;
      if ( sorted_y[i]->g == fr ) {
        if ( sorted_y[i]->chain )
        {
          sorted_y[i]->del = v + 1;
          res++;
        }
        continue;
      }
      sorted_y[i]->del = v; res++;
    }
    return res;
  }
  pair<trap *, int> *scan_rup(int from)
  {
    auto ct = sorted_y[from];
    pair<trap *, int> *myres = new pair<trap *, int>(ct, 0);
    if ( ct->g ) { myres->first = ct->g->first; return myres; };
    // check right cell
    if ( from < (int)sorted_y.size() - 1 && ct->y == sorted_y[from+1]->y && ct->x == sorted_y[from+1]->x -1 )
    {
      auto rres = scan_rup(from+1);
      if ( rres )
      {
        if ( rres->first->x > myres->first->x )
          myres->first = rres->first;
        myres->second += rres->second;
      }
    }
    // scan 2 adj upper cells
    for ( int i = from - 1; i >= 0; --i )
    {
      if ( sorted_y[i]->y == ct->y ) continue;
      if ( sorted_y[i]->y + 1 < ct->y ) break;
      if ( sorted_y[i]->x < ct->x ) break;
      if ( sorted_y[i]->x == ct->x + 1 || sorted_y[i]->x == ct->x )
      {
        auto next = scan_rup(i);
        if ( !next ) continue;
        if ( ct->y != next->first->y ) myres->second += rm_r(from, next->first->x, 2);
        myres->second += next->second;
        if ( next->first->x > myres->first->x )
        {
          myres->first = next->first;
          if ( sorted_y[i]->x == ct->x ) ct->chain = sorted_y[i];
        }
      }
    }
    ct->g = myres;
    // if ( ct->y != myres->first->y ) myres->second += rm_r2(from, myres->first->x, 3);
    return myres;
  }
  int rm_gen()
  {
    if ( sorted_y.size() < 2 ) return 0;
    int res = 0;
    for ( int i = (int)sorted_y.size() - 1; i >= 0; --i )
    {
      if ( sorted_y[i]->g || sorted_y[i]->del ) continue;
      pair<trap *, int> *sres = scan_rup(i);
      if ( !sres || sres->first == sorted_y[i] ) continue;
      res += sres->second;
#ifdef DEBUG
 printf("rm_gen %d %d X %d\n", sorted_y[i]->y, sorted_y[i]->x, sres->first->x);
#endif
    }
    return res;
  }
  void dump_map()
  {
    int cl = 1;
    string str;
    for ( int i = 0; i < (int)sorted_y.size(); ++i )
    {
      if ( sorted_y[i]->y != cl )
      {
        if ( !str.empty() ) printf("%s\n", str.c_str());
        if ( sorted_y[i]->y > cl + 1 ) printf("\n");
        cl = sorted_y[i]->y;
        str.clear();
      }
      if ( str.empty() ) { str.resize(N, ' '); }
      char c = 'X';
      switch(sorted_y[i]->del)
      {
        case 1: c = 'd'; break;
        case 2: c = 'r'; break;
        case 3: c = '?'; break;
        case 4: c = '+'; break;
      }
      str.at(sorted_y[i]->x - 1) = c;
    }
    if ( !str.empty() )
      printf("%s\n", str.c_str());
  }
  void prepare()
  {
    for ( size_t i = 0; i < traps.size(); ++i )
    {
      sorted_y.push_back(&traps[i]); sorted_x.push_back(&traps[i]);
    }
    sort(sorted_y.begin(), sorted_y.end(), [](const trap *a, const trap *b) { return a->y == b->y ? a->x < b->x : a->y < b->y; });
    sort(sorted_x.begin(), sorted_x.end(), [](const trap *a, const trap *b) { return a->x == b->x ? a->y < b->y : a->x < b->x; });
    rm_odd_x(); rm_odd_y(); rm_gen();
#ifdef DEBUG
 dump_map();
#endif
    // remove marked for deletion
    auto yend = remove_if(sorted_y.begin(), sorted_y.end(), [](const trap *a) { return a->del; });
    sorted_y.erase(yend, sorted_y.end());
    auto xend = remove_if(sorted_x.begin(), sorted_x.end(), [](const trap *a) { return a->del; });
    sorted_x.erase(xend, sorted_x.end());
  }
  void propagate(trap *t, size_t from)
  {
    for ( size_t i = from + 1; i < sorted_y.size(); ++i )
    {
      if ( sorted_y[i]->y >= t->y && sorted_y[i]->x >= t->x )
      {
        IT dist = numberOfPaths(sorted_y[i]->y - t->y + 1, sorted_y[i]->x - t->x + 1);
        IT diff = t->from_s * dist % mod;
// to test perl version with bignum
//    if ( sorted_y[i]->y == 56 && sorted_y[i]->x == 96 ) printf("sub from %d %d old %s - %s\n", t->y, t->x, 
//      sorted_y[i]->from_s.get_str().c_str(), diff.get_str().c_str());
        sorted_y[i]->from_s -= diff;
        if ( sorted_y[i]->from_s < 0 ) sorted_y[i]->from_s += mod;
      }
    }
  }
  void calc()
  {
    for ( auto t: sorted_y )
    {
      t->from_s = numberOfPaths(t->y, t->x);
      t->to_t = numberOfPaths(N - t->y + 1, N - t->x + 1);
    // if ( t->y == 56 && t->x == 96 ) printf("init %d %d from_s %s to_t %s\n", t->y, t->x, t->from_s.get_str().c_str(), t->to_t.get_str().c_str());
    }
printTime("init");
    for ( size_t i = 0; i < sorted_y.size(); ++i )
      propagate(sorted_y[i], i);
printTime("propagate");
    IT res = numberOfPaths(N, N);
    for ( size_t i = 0; i < sorted_y.size(); ++i )
    {
      res -= sorted_y[i]->from_s * sorted_y[i]->to_t % mod;
      if ( res < 0 ) res += mod;
    }
    printf("%ld\n", res);
  }
};

int main(int argc, char **argv)
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  sq g(n);
  fact[0] = 1;
  for(int i = 1; i <= n + n; i++) {
    fact[i] = (fact[i - 1] * i) % mod;
  }
  for ( int i = 0; i < m; ++i )
  {
    int a,b;
    cin>>a>>b;
    g.put_trap(a,b);
  }
  g.prepare();
printTime("prepare");
  g.calc();
printTime("res");
#ifdef TEST
  if ( g_dump_rem )
    g.dump_rem();
#endif
}