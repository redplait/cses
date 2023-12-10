#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <stack>
#include <algorithm>

// solution for Advertisement
// I remember this task - https://www.eolymp.com/en/problems/383
using namespace std;
// first - index, second - height
typedef pair<int, int> bar;

int main()
{
  int n;
  cin>>n;
  stack<bar> s;
  s.push({0,-1});
  int64_t res = 0;
  for(int i = 1; i <= n + 1; i++)
  {
    int h = 0;
    if ( i <= n ) cin>>h;
    int x = i;
    while (h <= s.top().second)
    {
      x = s.top().first;
      int hPrev = s.top().second;
      s.pop();
      int64_t area = 1LL * hPrev * (i - x);
      if (area > res) res = area;
    }
    s.push({x,h});
  }
  printf("%ld\n", res);
}