#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

// solution for Hamming Distance
// distance in just XOR between couple of numbers, bcs k < 30 we can use ordinary int
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

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, k;
  cin>>n>>k;
  vector<int> numbers(n);
  for ( int i = 0; i < n; i++ )
  {
    int res = 0;
    string s;
    cin>>s;
    for ( char c: s )
    {
      res <<= 1;
      if ( c == '1' ) res |= 1;
    }
    numbers[i] = res;
  }
printTime("scan");
  int res = k;
  for ( int i = 0; i < n; i++ )
   for ( int j = i + 1; j < n; j++ )
   {
     int diff = numbers[i] ^ numbers[j];
     res = min( res, __builtin_popcount(diff));
     if ( !res ) {
       puts("0");
       return 0; // we really found minimum
     }
   }
  printf("%d\n", res);
printTime("res");
}
