#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <algorithm>

// solution for Counting Patterns
// KMP ripped from https://web.snauka.ru/issues/2015/02/46825
// too slow :-(

using namespace std;

int *calc_kmp(string &s)
{
  int i, n = s.length(), t;
  int *br = new int[n];
  br[0] = 0;
  for (i = 1; i < n; i++) {
    t = br[i - 1];
    while ((t > 0) && (s[i] != s[t])) t = br[t - 1];
    if (s[i] == s[t]) {
      br[i] = t + 1;
    } else {
      br[i] = 0;
    }
  }
  return br;
}

int KMP(string &str, string &sub, int* br)
{
  int res = 0, poz = 0;
  for (int i = 0; i < (int)str.size(); i++) {
    while (poz == (int)sub.size() || (poz > 0 && sub[poz] != str[i])) {
      poz = br[poz - 1];
      if (sub.size() - poz > str.size() - i) break;
    }
    if (str[i] == sub[poz]) poz++;
    if (poz == (int)sub.size()) res++;
  }
  return res;
}

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  string s;
  cin>>s;
  int *p = calc_kmp(s);
  int k;
  cin>>k;
  for ( int i = 0; i < k; i++ )
  {
    string pat;
    cin>>pat;
    printf("%d\n", KMP(s, pat, p));
  }
}