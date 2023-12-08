#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <assert.h>

// solution for Giant Pizza
// actually this is task for satisfiability so uses 2SAT solver
// -std=c++14

using namespace std;

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

template <class E> struct csr {
    std::vector<int> start;
    std::vector<E> elist;
    csr(int n, const std::vector<std::pair<int, E>>& edges)
        : start(n + 1), elist(edges.size()) {
        for (auto e : edges) {
            start[e.first + 1]++;
        }
        for (int i = 1; i <= n; i++) {
            start[i] += start[i - 1];
        }
        auto counter = start;
        for (auto e : edges) {
            elist[counter[e.first]++] = e.second;
        }
    }
};

struct scc_graph {
   int N;
   // first - from, second - to
   std::vector<std::pair<int, int>> edges;
   scc_graph(int n) : N(n) {}
   inline int num_vertices() { return N; }
   void add_edge(int from, int to) { edges.push_back({from, to}); }
   // @return pair of (# of scc, scc id)
   std::pair<int, std::vector<int>> scc_ids()
   {
     auto g = csr<int>(N, edges);
     int now_ord = 0, group_num = 0;
     std::vector<int> visited, low(N), ord(N, -1), ids(N);
     auto dfs = [&](auto self, int v) -> void {
        low[v] = ord[v] = now_ord++;
        visited.push_back(v);
        for (int i = g.start[v]; i < g.start[v + 1]; i++) {
            auto to = g.elist[i];
            if (ord[to] == -1) {
                self(self, to);
                low[v] = std::min(low[v], low[to]);
            } else {
                low[v] = std::min(low[v], ord[to]);
            }
        }
        if (low[v] == ord[v]) {
            while (true) {
                int u = visited.back();
                visited.pop_back();
                ord[u] = N;
                ids[u] = group_num;
                if (u == v) break;
            }
            group_num++;
        }
     };
     for (int i = 0; i < N; i++) {
        if (ord[i] == -1) dfs(dfs, i);
     }
     for (auto& x : ids) {
        x = group_num - 1 - x;
     }
     return {group_num, ids};
   }
 
    std::vector<std::vector<int>> scc() {
        auto ids = scc_ids();
        int group_num = ids.first;
        std::vector<int> counts(group_num);
        for (auto x : ids.second) counts[x]++;
        std::vector<std::vector<int>> groups(ids.first);
        for (int i = 0; i < group_num; i++) {
            groups[i].reserve(counts[i]);
        }
        for (int i = 0; i < N; i++) {
            groups[ids.second[i]].push_back(i);
        }
        return groups;
    }
};

// Reference:
// B. Aspvall, M. Plass, and R. Tarjan,
// A Linear-Time Algorithm for Testing the Truth of Certain Quantified Boolean
// Formulas
struct two_sat: public scc_graph {
   int n_;
   two_sat(int n) : scc_graph(2 * n), _n(n), _answer(n) {}
 
    void add_clause(int i, bool f, int j, bool g) {
        assert(0 <= i && i < _n);
        assert(0 <= j && j < _n);
        add_edge(2 * i + (f ? 0 : 1), 2 * j + (g ? 1 : 0));
        add_edge(2 * j + (g ? 0 : 1), 2 * i + (f ? 1 : 0));
    }
    bool satisfiable() {
        auto id = scc_ids().second;
        for (int i = 0; i < _n; i++) {
            if (id[2 * i] == id[2 * i + 1]) return false;
            _answer[i] = id[2 * i] < id[2 * i + 1];
        }
        return true;
    }
    std::vector<bool> &answer() { return _answer; }
 
  private:
    int _n;
    std::vector<bool> _answer;
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  two_sat tsat(m);
  while( n-- )
  {
    int x1, x2;
    char c1, c2;
    cin>>c1>>x1>>c2>>x2;
    tsat.add_clause(x1-1, c1 == '+', x2-1, c2 == '+');
  }
  if ( !tsat.satisfiable() ) puts("IMPOSSIBLE");
  else {
    auto res = tsat.answer();
    for ( auto b: res ) printf("%c ", b ? '+' : '-');
    printf("\n");
  }
}
