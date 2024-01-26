#include <bits/stdc++.h>
using namespace std;

#pragma GCC optimize("-Ofast")

// solution for Word Combinations
// version with trie - it is too big and cannot be allocated on stack so I made it global
// 0.13s on test 6

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

int trie[1000001][26]; // total length of the words is at most 10^6
bool stop[1000001];

struct wc
{
  string &S;
  int N, index = 0;
  vector<int> max_len;
  vector<int64_t> dp;
  void insert(const char *s, int size)
  {
    int node = 0;
    for (int i = 0; i < size; i++)
    {
      int c = s[i]-'a';
      if (!trie[node][c]) trie[node][c] = ++index;
      node = trie[node][c];
    }
    stop[node] = 1;
  }
  wc(string &s): S(s), N((int)s.size()), max_len(26), dp(N)
  {
    int k;
    cin>>k;
    for (int i = 0; i < k; i++ )
    {
      string w;
      cin>>w;
      int size = (int)w.size();
      int c = w[0] - 'a';
      max_len[c] = max(max_len[c], size);
      insert(w.c_str(), size);
    }
#ifdef DEBUG
 for ( int i = 0; i < 26; i++ ) printf("[%d] %d\n", i, max_len[i]);
#endif
  }
  int64_t process_letter(int i)
  {
    if ( dp[i] ) return dp[i];
    int initial = S[i] - 'a';
    if ( !max_len[initial] ) return 0; // no words starting with letter at index i
    int node = 0;
    int64_t res = 0;
    for ( int j = i; j < N; j++ )
    {
      if ( j - i + 1 > max_len[initial] ) break;    // no such long strings starting with letter initial
      // search in trie
      int c = S[j] - 'a';
      if (!trie[node][c]) return res;
      node = trie[node][c];
      if ( stop[node] )
      {
        if ( j == N - 1 ) res++;
        else if ( dp[j+1] ) { res += dp[j+1]; res %= mod; }
      }
    }
#ifdef DEBUG
 printf("i %d c %d %ld\n", i, initial, res);
#endif
    return res;
  }
  int64_t calc()
  {
    for ( int i = N - 1; i >= 0; i-- ) dp[i] = process_letter(i);
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