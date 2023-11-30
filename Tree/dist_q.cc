#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

// solution for Distance Queries
// we need cache all LCA, given that maximum nodes is 200000 then logK = 18
using namespace std;
const int logK = 18;

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
 vector<int> parents[logK];
 vector<int> dist, in, out;
 int timer = 0;
 rt(int n)
  : g(n), dist(n), in(n), out(n)
 {
   for ( int i = 0; i < logK; ++i ) parents[i].resize(n);
 }
 void read(int n)
 {
   for ( int i = 1; i < n; ++i )
   {
     int a, b;
     cin>>a>>b; a--; b--;
     g[a].push_back(b);
     g[b].push_back(a);
   }
   dfs(0, 0);
 }
 void dfs(int n, int p)
 {
   in[n] = ++timer;
   dist[n] = dist[p] + 1;
   parents[0].at(n) = p;
   for ( int i = 1; i < logK; i++)
     parents[i].at(n) = parents[i-1].at(parents[i-1].at(n));
   for ( int i: g[n] )
   {
     if ( i == p ) continue; // skip parent of this node
     dfs(i, n);
   }
   out[n] = ++timer;
 }
 inline bool is_anc(int u, int v)
 {
   return in[u] <= in[v] && out[u] >= out[v];
 }
 int lca(int u, int v)
 {
   if ( is_anc(u, v) ) return u;
   if ( is_anc(v, u) ) return v;
   for ( int i = logK - 1; i >= 0; --i )
     if( !is_anc(parents[i].at(u), v) )
        u = parents[i].at(u);
    return parents[0].at(u);
 }
 int calc(int a, int b)
 {
   // length is dist from a to root + from b to root so we need sub 2 * dist of LCA a & b
   return dist[b] + dist[a] - 2 * dist[ lca(a, b) ];
 }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, q;
  cin>>n>>q;
  rt t(n);
  t.read(n);
printTime("read");
  while( q-- )
  {
    int a, b;
    cin>>a>>b;
    printf("%d\n", t.calc(a-1, b-1));
  }
printTime("res");
}