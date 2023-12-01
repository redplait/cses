#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>

// solution for Tree Distances II
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

struct rt {
 vector<vector<int> > g;
 vector<int> sizes;
 vector<int64_t> up, down;
 int N;
 rt(int n)
  : g(n), sizes(n), up(n), down(n), N(n)
 { }
 void read()
 {
   for ( int i = 1; i < N; ++i )
   {
     int a, b;
     cin>>a>>b; a--; b--;
     g[a].push_back(b);
     g[b].push_back(a);
   }
 }
 // calc distances for down nodes
 void dfs(int u, int parent = -1)
 {
   sizes[u] = 1;
   for(int v : g[u])
   {
     if (v == parent) continue;
     dfs(v, u);
     sizes[u] += sizes[v];
     down[u] += down[v] + sizes[v];
   }
 }
 // calc distances for upper nodes
 void dfs_up(int u, int parent = -1)
 {
    if (parent != -1)
      up[u] = (up[parent]+down[parent]) + N - (2*sizes[u]+down[u]);
    for(int v : g[u]) if (v != parent) dfs_up(v, u);
 }
 void calc()
 {
   dfs(0);
   dfs_up(0);
 }
 void dump()
 {
   for ( int i = 0; i < N; i++ )
    printf("%ld ", down[i] + up[i]);
   printf("\n");
 }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n;
  cin>>n;
  rt t(n);
  t.read();
printTime("read");
  t.calc();
printTime("dfs");
  t.dump();
}