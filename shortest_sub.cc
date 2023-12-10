#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <vector>

// solution for Shortest Subsequence
using namespace std;

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  string s, res;
  cin>>s;
  int mask = 0;
  int cnt = 0;
  for ( char c: s )
  {
    int idx;
    switch(c)
    {
      case 'A': idx = 1; break;
      case 'C': idx = 2; break;
      case 'G': idx = 4; break;
      case 'T': idx = 8; break;
      default: puts("bad char"); return 3;
    }
    if ( !(mask & idx) )
    {
      mask |= idx;
      cnt++;
      if ( 4 == cnt )
      {
        cnt = mask = 0;
        res.push_back(c);
      }
    }
  }
  printf("%s", res.c_str());
  // and finally add last symbol not in mask
  if ( !(mask & 1) ) printf("A\n");
  else if ( !(mask & 2) ) printf("C\n");
  else if ( !(mask & 4) ) printf("G\n");
  else printf("T\n");
}