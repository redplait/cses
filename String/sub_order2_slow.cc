#include <bits/stdc++.h>
using namespace std;

/* slow solution for Substring Order II
   test-case for strings baabaa & baa
 k starts from 1, last n * (n + 1) / 2
 1-4 a             1-2 a
 5,6 aa            3   aa
 7   aab           4   b
 8   aaba          5   ba
 9   aabaa         6   baa
 10  ab
 11  aba
 12  abaa
 13,14 b
 15,16 ba
 17,18 baa
 19    baab
 20    baaba
 21    baabaa
*/

struct sa
{
  string &S;
  int N;
  vector<int> comb;
  struct letter
  {
    int pos = -1, cnt = 0;
    int64_t sum = 0;
  } letters[26];
  typedef pair<int, int> lp;
  typedef vector<lp> cpos;
#ifdef DEBUG
  void dump_cpos(cpos &c, int n)
  {
    for ( int i = 0; i < n; i++ )
    {
      if ( c[i].second >= N ) break;
      printf("%d> pos %d %s\n", i, c[i].second, S.c_str() + c[i].second);
    }
  }
  void dump_letters()
  {
    for ( int i = 0; i < 26; i++ )
    {
      if ( letters[i].pos == -1 ) continue;
      printf("[%d] pos %d sum %ld cnt %d\n", i, letters[i].pos, letters[i].sum, letters[i].cnt);
    }
  }
  void dump_list(list<int> &res)
  {
    for ( auto i: res ) printf("pos %d comb %d\n", i, comb[i]);
  }
#endif
  sa(string &s): S(s), comb( s.size() )
  {
    N = (int)s.size();
    // fill comb
    for ( int i = N - 1; i >= 0; i-- ) comb[i] = N - i;
#ifdef DEBUG
//    for ( int i = 0; i < N; i++ )
//     printf("[%d] %c %d\n", i, S[i], comb[i]);
#endif
    // fill letters
    for ( int i = 0; i < N; i++ )
    {
      int c = s[i] - 'a';
      if ( letters[c].pos == -1 ) letters[c].pos = i;
      letters[c].sum += comb[i];
      letters[c].cnt++;
    }
#ifdef DEBUG
 dump_letters();
#endif
  }
  // fill list with all positions of letter with index next
  void fill_list(int next, list<int> &res, int &llen)
  {
    llen = 0;
    for ( int i = letters[next].pos; i < N; i++ )
    {
      if ( next != S[i] - 'a' ) continue;
      llen++;
      if ( 1 == comb[i] ) break;
      res.push_back(i);
    }
  }
  // called when all letters after next if the same
  void find_same(int n, int64_t k)
  {
    n = N - n;
#ifdef DEBUG
printf("find_same k %ld res %d\n", k, n);
#endif
    char c = S[n];
    while(k && n)
    {
      printf("%c", c);
      if ( k <= n ) return;
      k -= n;
      n--;
    }
  }
  struct customLess {
    string &s;
    customLess(string &in_s): s(in_s) {}
    bool operator()(const lp *a, const lp *b) {
      int l1 = a->second - a->first, l2 = b->second - b->first;
      if ( l1 < l2 ) return true;
      if ( l1 > l2 ) return false;
      return s[a->second] > s[b->second];
    }
    inline bool eq(const lp *a, const lp *b)
    {
      return (a->second - a->first == b->second - b->first) && (s[a->second] == s[b->second]);
    }
  };
  void find_slow(const list<int> &res, int64_t k)
  {
#ifdef DEBUG
 printf("find_slow k %ld res %ld\n", k, res.size());
#endif
    int n = (int)res.size();
    cpos cp(n);
    int i = 0;
    for ( auto ri = res.cbegin(); ri != res.cend(); ++ri, ++i )
      cp[i].first = cp[i].second = *ri + 1;
#ifdef DEBUG
dump_cpos(cp, n);
#endif
    customLess cl(S);
    vector<lp *> popped;
    priority_queue<lp *, vector<lp *>, customLess> q(cl);
    // fill pq
    for ( auto &pos: cp ) if ( pos.second < N ) q.push(&pos);
    while(n && k)
    {
      auto top = q.top();
      q.pop();
#ifdef DEBUG
 printf("pop %d %c k %ld size %ld\n", top->first, S[top->second], k, q.size());
#endif
      if ( q.empty() )
      { // this is last string
        printf("%s\n", S.substr(top->first, top->second + k - top->first).c_str());
        return;
      }
      int top_cnt = 1;
      while(!q.empty()) {
        auto curr = q.top();
        if ( !cl.eq(top, curr) ) break;
        curr->second++; k--;
        if ( !k ) {
#ifdef DEBUG
 printf("next pos %d k %ld\n", curr->first, k);
#endif
          printf("%s\n", S.substr(top->first, top->second + 1 - top->first).c_str());
          return;
        }
        top_cnt++;
        q.pop();
        if ( curr->second < N ) popped.push_back(curr);
      }
      if ( 1 == top_cnt )
      {
        int l = N - top->second;
        if ( k <= l )
        {
#ifdef DEBUG
 printf("top_cnt k %ld l %d %s\n", k, l, S.c_str() + top->first);
#endif
          printf("%s\n", S.substr(top->first, top->second + k - top->first).c_str());
          return;
        }
        k -= l;
        top->second = N;
      } else {
        k--;
        if ( !k ) {
#ifdef DEBUG
 printf("last k %ld\n", k);
#endif
           printf("%s\n", S.substr(top->first, top->second + 1 - top->first).c_str());
           return;
        }
        top->second++;
        q.push(top);
        for ( auto p: popped ) q.push(p);
        popped.clear();
      }
#ifdef DEBUG
      dump_cpos(cp, n);
#endif
    }
  }
  void calc(int64_t k)
  {
    // check if k is last index
    if ( k == N * (N + 1) / 2 )
    {
      puts(S.c_str());
      return;
    }
    int next;
    // find initial letter of k-substring
    for ( int i = 0; i < 26; i++ )
    {
      if ( letters[i].pos == -1 || !letters[i].sum ) continue;
      next = i;
      if ( k <= letters[i].sum ) { printf("%c", i + 'a'); break; }
      k -= letters[i].sum;
    }
    // check if this is only letter
    if ( 1 == letters[next].cnt )
    {
      printf("%s\n", S.substr(letters[next].pos + 1, k - 1).c_str());
      return;
    }
    // check if k-substring in 1-letter
    if ( k <= letters[next].cnt ) return;
    k -= letters[next].cnt;
#ifdef DEBUG
    printf("next %d k %ld pos %d\n", next, k, letters[next].pos);
#endif
    int llen;
    list<int> l;
    fill_list(next, l, llen);
#ifdef DEBUG
    dump_list(l);
#endif
    if ( N - letters[next].pos == llen )
      find_same(l.front() + 1, k);
    else
      find_slow(l, k);
  }
};

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
  ios::sync_with_stdio(0); cin.tie(0);
  string s;
  cin >> s;
  sa sa(s);
  int64_t k;
  cin>>k;
  sa.calc(k);
printTime("res");
}