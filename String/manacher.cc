#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

// solution for Longest Palindrome
// ripped from https://codeforces.com/blog/entry/12143
using namespace std;

struct manacher
{
  vector<int> p[2];
  int best_l = 0, best_r = 0;
  manacher(string &s)
  {
    int n = (int)s.size();
    p[0].resize(n, 0); p[1].resize(n, 0);
    for( int z=0,l=0,r=0; z<2; z++,l=0,r=0)
    {
        for(int i=0;i<n;i++)
        {
            if(i<r) p[z][i]=min(r-i+!z,p[z][l+r-i+!z]);
            while(i-p[z][i]-1>=0 && i+p[z][i]+1-!z<n && s[i-p[z][i]-1]==s[i+p[z][i]+1-!z]) p[z][i]++;
            if ( 2 * p[z][i] - !z >= best_r - best_l + 1 )
            { best_l = i-p[z][i]; best_r = i+p[z][i]-!z; }
            if (i+p[z][i]-!z>r)
            { l=i-p[z][i],r=i+p[z][i]-!z; }
        }
    }
#ifdef DEBUG
printf("%d %d\n", best_l, best_r);
#endif
 printf("%s\n", s.substr(best_l, best_r - best_l + 1).c_str());
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  string s;
  cin>>s;
  manacher tmp(s);
}