#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <limits.h>
#include <iostream>
#include <vector>
#include <queue>

// solution for School Dance
// stealed from https://judge.yosupo.jp/submission/52112
using namespace std;

struct bipartite_matching {
    int n_left, n_right, flow = 0;
    vector<vector<int> > g;
    vector<int> match_from_left, match_from_right;

    bipartite_matching(int _n_left, int _n_right)
        : n_left(_n_left),
          n_right(_n_right),
          g(_n_left),
          match_from_left(_n_left, -1),
          match_from_right(_n_right, -1),
          dist(_n_left) {}

    inline void add(int u, int v) { g[u].push_back(v); }

    vector<int> dist;

    void bfs() {
        queue<int> q;
        for (int u = 0; u < n_left; ++u) {
            if (!~match_from_left[u])
                q.push(u), dist[u] = 0;
            else
                dist[u] = -1;
        }
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (auto v : g[u])
                if (~match_from_right[v] && !~dist[match_from_right[v]]) {
                    dist[match_from_right[v]] = dist[u] + 1;
                    q.push(match_from_right[v]);
                }
        }
    }

    bool dfs(int u) {
        for (auto v : g[u])
            if (!~match_from_right[v]) {
                match_from_left[u] = v, match_from_right[v] = u;
                return true;
            }
        for (auto v : g[u])
            if (dist[match_from_right[v]] == dist[u] + 1 &&
                dfs(match_from_right[v])) {
                match_from_left[u] = v, match_from_right[v] = u;
                return true;
            }
        return false;
    }
    int get_max_matching() {
        while (true) {
            bfs();
            int augment = 0;
            for (int u = 0; u < n_left; ++u)
                if (!~match_from_left[u]) augment += dfs(u);
            if (!augment) break;
            flow += augment;
        }
        return flow;
    }
    void get_edges(vector<pair<int, int> > &ans) {
        for (int u = 0; u < n_left; ++u)
            if (match_from_left[u] != -1)
                ans.emplace_back(u, match_from_left[u]);
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

int main() {
    ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
    int n_left, n_right, k;
    cin>>n_left>>n_right>>k;
    bipartite_matching matching(n_right, n_left);
    for ( int i = 0; i < k; i++ )
    {
        if ( cin.eof() ) break;
        int u, v;
        cin>>u>>v;
        matching.add(v-1, u-1);
    }
printTime("read");
    printf("%d\n", matching.get_max_matching());
    vector<pair<int, int> > ans;
    matching.get_edges(ans);
printTime("calc");
    for (auto &p : ans ) printf("%d %d\n", p.second+1, p.first+1);
}