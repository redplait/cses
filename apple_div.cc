#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <vector>

// solution for Apple Division
// complexity is O( 2 ^ n)
using namespace std;
typedef pair<int64_t, int64_t> grp;

struct adiv
{
  int64_t res = LONG_MAX;
  int N;
  vector<int> data;
  adiv(int n): N(n), data(n)
  {
    for ( int i = 0; i < N; i++ ) cin>>data[i];
  }
  void rec_exp(grp &g, int i)
  {
    // base case - we at last apple, calc minimal difference
    if ( i == N - 1 )
    {
      res = min(res, min(abs(g.first + data[i] - g.second), abs(g.first - data[i] - g.second)));
      return;
    }
    // put current apple to first group
    grp my{ g.first + data[i], g.second };
    rec_exp(my, i+1);
    // put current apple to second group
    my.first -= data[i];
    my.second += data[i];
    rec_exp(my, i+1);
  }
  int64_t calc()
  {
    if ( !N ) return 0;
    if ( 1 == N ) return data[0];
    grp g{ data[0], 0 };
    rec_exp(g, 1);
    g = { 0, data[0] };
    rec_exp(g, 1);
    return res;
  }
};

int main()
{
  int n;
  cin>>n;
  adiv a(n);
  printf("%ld\n", a.calc());
}