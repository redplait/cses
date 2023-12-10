#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm> // for reverse

// Course Schedule II
// topological sorting don`t work with this task
using namespace std;

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  vector<int> degrees(n);
  vector<int> res;
  vector<vector<int> > g(n);
  priority_queue<int> pq;
  for(int i = 0; i < m; i++)
  {
    int a, b;
    cin>>a>>b;
    a--; b--;
    g[b].push_back(a);
    degrees[a]++;
  }
  for ( int i = 0; i < n; i++ )
    if ( !degrees[i] ) pq.push(i);
  while(!pq.empty())
  {
    int u = pq.top(); 
    pq.pop();
    res.push_back(u);
    for(auto v : g[u])
    {
       degrees[v]--;
       if( !degrees[v] ) pq.push(v);
    }
  }
  reverse(res.begin(), res.end());
  for ( auto v: res ) printf("%d ", 1+v);
  printf("\n");
};