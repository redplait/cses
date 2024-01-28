#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const int N = 1e5 + 5;

// solution for Substring Order II
// based on https://codeforces.com/contest/128/submission/72601519
// suffix links from https://cp-algorithms.com/string/suffix-automaton.html
// 0.09s on test 5

struct state {
  int len, link, next[26];
  ll cnt = 0, cnt2 = -1;
};

string s;
state st[2 * N];
ll k;
int n, sz, last;
vector<pair<int, int>> order;

void st_init() {
  st[0].len = 0;
  st[0].link = -1;
  sz++; last = 0;
}

void dfs(int u) {
  if (st[u].cnt2 != -1) return;
  st[u].cnt2 = st[u].cnt;
  for (int i = 0; i < 26; ++i)
  {
    if (!st[u].next[i]) continue;
    dfs(st[u].next[i]);
    st[u].cnt += st[st[u].next[i]].cnt;
  }
}

void st_extend(int c)
{
  int cur = sz++;
  st[cur].len = st[last].len + 1;
  st[cur].cnt = 1;
  order.emplace_back(st[cur].len, cur);
  int p = last;
  while (p != -1 && !st[p].next[c]) {
    st[p].next[c] = cur;
    p = st[p].link;
  }
  if (p == -1) {
    st[cur].link = 0;
  } else {
    int q = st[p].next[c];
    if (st[p].len + 1 == st[q].len) {
      st[cur].link = q;
    } else {
      int clone = sz++;
      st[clone].len = st[p].len + 1;
      st[clone].link = st[q].link;
      memcpy(st[clone].next, st[q].next, sizeof(st[q].next));
      order.emplace_back(st[clone].len, clone);
      while (p != -1 && st[p].next[c] == q) {
        st[p].next[c] = clone;
        p = st[p].link;
      }
      st[cur].link = st[q].link = clone;
    }
  }
  last = cur;
}

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
  ios::sync_with_stdio(false); cin.tie(0); cout.tie(0);
  cin >> s >> k;
  n = s.length(); k += n;
  st_init();
  for (int i = 0; i < n; ++i) st_extend(s[i] - 'a');
  sort(order.rbegin(), order.rend());
  for (auto &p: order) {
    st[st[p.second].link].cnt += st[p.second].cnt;
  }

  dfs(0);

  if (st[0].cnt < k) {
    cout << "No such line.";
    return 0;
  }

  int cur = 0;
  while (k > st[cur].cnt2)
  {
    k -= st[cur].cnt2;
    for (int i = 0; i < 26; ++i)
    {
      if (!st[cur].next[i]) continue;
      int j = st[cur].next[i];
      if (st[j].cnt < k) k -= st[j].cnt;
      else {
         putc((char)(i + 'a'), stdout);
         cur = j;
         break;
      }
    }
  }
printTime("\nres");
}