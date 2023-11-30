#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <limits.h>

// solution for Tree Isomorphism II
// ripped from https://codeforces.com/blog/entry/101010
// seems that method Centroid should return reference to result to avoid copy
// + isIsomorphic can have reference to t2 arg too

using namespace std;
const int MOD = 1e9 + 9;

struct Tree {
    vector<vector<int> > adj;
    vector<int> centroid;
    vector<int> sub;
    vector<int> id;
    vector<int64_t> powr;

    void dfs(int curNode, int prevNode) {
        sub[curNode] = 1;
        bool is_centroid = true;
        vector<pair<int, int> > nodes;
        for (int v: adj[curNode]) {
            if (v != prevNode) {
                dfs(v, curNode);
                sub[curNode] += sub[v];
                if (sub[v] > (int) adj.size() / 2) {
                    is_centroid = false;
                }
                nodes.emplace_back(id[v], v);
            }
        }
        sort(nodes.begin(), nodes.end());
        id[curNode] = 1;
        for (auto& p: nodes) {
            id[curNode] = ((powr[sub[p.second] + 1] * id[curNode]) % MOD + id[p.second]) % MOD;
        }
        id[curNode] *= 2;
        id[curNode] %= MOD;
        if ((int) adj.size() - sub[curNode] > (int) adj.size() / 2) {
            is_centroid = false;
        }
        if (is_centroid) centroid.push_back(curNode);
    }

    const vector<int> &Centroid() {
        dfs(0, -1);
        return centroid;
    }

    bool isIsomorphic(int root1, Tree &t2, int root2)
    {
        dfs(root1, root1);
        t2.dfs(root2, root2);
        sort(sub.begin(), sub.end());
        sort(t2.sub.begin(), t2.sub.end());
        return (id[root1] == t2.id[root2]);
    }
    void read(int n)
    {
      for (int i = 1; i < n; i++)
      {
        int u, v;
        cin >> u >> v;
        u--; v--;
        adj[u].push_back(v);
        adj[v].push_back(u);
      }
    }
    Tree(int n) : adj(n), sub(n), id(n), powr(n+1)
    {
        powr[0] = 1;
        for (int i = 1; i <= n; i++) {
            powr[i] = 2 * powr[i - 1];
            powr[i] %= MOD;
        }
    }
};

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int t;
    cin >> t;
    while (t--)
    {
        int N;
        cin >> N;
        Tree t1(N), t2(N);
        t1.read(N); t2.read(N);
        auto c1 = t1.Centroid();
        auto c2 = t2.Centroid();
        bool done = false;
        for (int i: c1)
        {
          for (int j: c2) {
            if (!done && t1.isIsomorphic(i, t2, j)) {
                puts("YES");
                done = true;
                break;
            }
          }
          if ( done ) break;
        }
        if (!done) puts("NO");
    }
}