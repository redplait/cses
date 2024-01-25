#include <bits/stdc++.h>
using namespace std;

#pragma GCC optimize("-Ofast")

// slow solution for Word Combinations
// timeout on test 4

#define mod 1000000007

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

struct wc
{
  string &S;
  int N;
  vector<int> max_len, min_len;
  vector<int64_t> dp;
  vector<string> storage;
  unordered_set<string_view> dict[26];
  wc(string &s): S(s), N((int)s.size()), max_len(26), min_len(26, INT_MAX), dp(N)
  {
    int k;
    cin>>k;
    storage.resize(k);
    for (int i = 0; i < k; i++ )
    {
      cin>>storage[i];
      if ( string::npos == S.find(storage[i]) ) continue;
      int size = (int)storage[i].size();
      int c = storage[i][0] - 'a';
      max_len[c] = max(max_len[c], size);
      min_len[c] = min(min_len[c], size);
      dict[c].insert({storage[i].c_str(), size});
    }
#ifdef DEBUG
 for ( int i = 0; i < 26; i++ ) printf("[%d] %d\n", i, max_len[i]);
#endif
  }
  int64_t process_letter(int i)
  {
    if ( dp[i] ) return dp[i];
    int c = S[i] - 'a';
    if ( !max_len[c] ) return 0; // no words starting with letter at index i
    for ( int j = i; j < N; j++ )
    {
      if ( j - i + 1 < min_len[c] ) continue;
      if ( j - i + 1 > max_len[c] ) break;
      string_view tmp(S.c_str() + i, j - i + 1);
      // string tmp = S.substr(i, j - i + 1);
      auto fi = dict[c].find(tmp);
      if ( fi == dict[c].end() ) continue;
      if ( j == N - 1 ) dp[i]++;
      else {
        int64_t res = process_letter(j + 1);
        if ( res ) { dp[i] += res; dp[i] %= mod; }
      }
    }
#ifdef DEBUG
 printf("i %d c %d %ld\n", i, c, dp[i]);
#endif
    return dp[i];
  }
  int64_t calc()
  {
    for ( int i = N - 1; i >= 0; i-- ) process_letter(i);
    return dp[0];
  }
};

int main()
{
  ios::sync_with_stdio(0); cin.tie(0);
  string s;
  cin>>s;
  wc w(s);
printTime("read");
  printf("%ld\n", w.calc());
printTime("calc");
}