#include <bits/stdc++.h>

// solution for Special Substrings
// based on DP with caching each frequency array
// 0.18s on test 7

using namespace std;
typedef long long ll;

map<char,int> ch_pos;
map<vector<int>,ll> dp;

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  string S;
  cin>>S;
  int M = 0;
  // fill char initial positions
  for (char c: S)
  {
    auto chi = ch_pos.find(c);
    if ( chi == ch_pos.end() )
    {
      ch_pos[c] = M++;
      if ( 26 == M ) break;
    }
  }
  ll ans = 0;
  vector<int> freq(M);
  dp[freq]++;
  for (char c: S)
  {
    int cp = ch_pos[c];
    freq[cp]++;
    if ( all_of(freq.begin(), freq.end(), [](int i) { return i > 0; }) )
    {
      for (int j = 0; j < M; j++) freq[j]--;
    }
    ans += dp[freq];
    dp[freq]++;
  }
  printf("%lld\n", ans);
}