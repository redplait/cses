#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <list>
#include <unordered_set>

// slow solution for Forbidden Cities
// problem with naive algo is that for example on test 9 we have 24202 cutpoints and 31890 SCC 
// so in average we must run wave on 31890 nodes for condences graph
// One solution is to merge chains - where degrees of adjacent vertices < 3 and chain will have max 2 edges
// then on test 9 we will have only 15474 SCC total
// Next observation is that we can check if condensed graph is connected. If so then if forbidden city nor cutpoint
// nor in chain then answer is yes. And if condensed graph is disjoined than answer is SCC index of A == SCC index of B
// Anyway this is too slow solution - on my notebook it running with gcc -O3:
// test 6: 79s with 23713 SCC
// test 7: 61s with 22918 SCC
// test 8: 15s with 18822 SCC
// test 9: 3.5s with 15474 SCC

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

struct node
{
  int n;
  vector<int> edges;
  inline int degree() const
  {
    return edges.size();
  }
  int tin, fup, css = -1, chain_order;
  bool cutpoint = false;
};

struct cond_node
{
  int chain = 0;
  int left = -1;
  int right = -1;
  int cond_scc = -1;
  unordered_set<int> edges;
};

struct graph
{
  int N;
  int cp_size = 0;
  int cond_size = 0;
  int wave_cnt;
  int cond_connected = 0;
  vector<node> nodes;
  vector<cond_node> cond;
  char *visited;
  graph(int n)
   : N(n), nodes(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
    visited = (char *)malloc(N);
    reset();
  }
  void reset_cond()
  {
    wave_cnt = 0;
    memset(visited, 0, cond.size());
  }
  void reset()
  {
    memset(visited, 0, N);
  }
  inline void add_edge(int a, int b)
  {
    node &n = nodes[a-1];
    n.edges.push_back(b-1);
    node &m = nodes[b-1];
    m.edges.push_back(a-1);
  }
  int timer = 0;
  // based on https://e-maxx.ru/algo/cutpoints
  // complexity O(V + E)
  void dfs(int v, int p = -1)
  {
    visited[v] = 1;
    nodes[v].tin = nodes[v].fup = timer++;
    int children = 0;
    for ( int to: nodes[v].edges ) 
    {
      if (to == p) // cycle to itself
        continue;
      if (visited[to])
        nodes[v].fup = min(nodes[v].fup, nodes[to].tin);
      else 
      {
        dfs (to, v);
        nodes[v].fup = min(nodes[v].fup, nodes[to].fup);
        if (nodes[to].fup >= nodes[v].tin && p != -1)
          { ++cp_size; nodes[v].cutpoint = true; }
        ++children;
      }
    }
    if (p == -1 && children > 1)
      { ++cp_size; nodes[v].cutpoint = true; }
  }
  void dfs_css(int v, int id)
  {
    visited[v] = 1;
    nodes[v].css = id;
    for ( int to: nodes[v].edges )
    {
      if ( visited[to] || nodes[to].cutpoint ) continue;
      dfs_css(to, id);
    }
  }
  int scan_left(int parent, int v, list<int> &cand)
  {
    if ( nodes[v].degree() > 2 ) return v;
    visited[v] = 1;
    if ( nodes[v].degree() == 1 ) { cand.push_front(v); return -1; }
    cand.push_front(v);
    // find next not parent
    int next = nodes[v].edges[0];
    if ( next == parent ) next = nodes[v].edges[1];
    return scan_left(v, next, cand);
  }
  int scan_right(int parent, int v, list<int> &cand)
  {
    if ( nodes[v].degree() > 2 ) return v;
    visited[v] = 1;
    if ( nodes[v].degree() == 1 ) { cand.push_back(v); return -1; }
    cand.push_back(v);
    int next = nodes[v].edges[0];
    if ( next == parent ) next = nodes[v].edges[1];
    return scan_right(v, next, cand);
  }
  void condense()
  {
    // 1) find cut-points
    for ( int i = 0; i < N; ++i )
    {
      if ( !visited[i] ) // graph can have several disjointed parts
       dfs(i);
    }
#ifdef TIME
  printTime("cutpoints");
  printf("cp_size %d\n", cp_size);
#endif
    // 2) try to find chains
    reset();
    int scc = 0;
    for ( int i = 0; i < N; ++i )
    {
      if ( visited[i] || !nodes[i].cutpoint || nodes[i].degree() != 2 ) continue;
      // some candidate - cutpoint with degree 2
      list<int> cand;
      cand.push_back(i);
      int l = scan_left(i, nodes[i].edges[0], cand);
      int r = scan_right(i, nodes[i].edges[1], cand);
      if ( cand.size() == 1 ) continue;
#ifdef DEBUG
printf("cand %d\n", (int)cand.size());
#endif
      // fill order of nodes inside chain
      int order = 0;
      for ( int j: cand )
      {
        nodes[j].chain_order = order++;
        nodes[j].css = scc;
      }
      cond_node chain;
      chain.chain = 1;
      // fill left and right edges
      chain.left = l;
      chain.right = r;
      cond.push_back(chain);
      scc++;
    }
printTime("chains");
    // 3) fill SCC
    reset();
    for ( int i = 0; i < N; ++i )
    {
      if ( nodes[i].cutpoint )
      {
        if ( nodes[i].css == -1 ) nodes[i].css = scc++;
        continue;
      }
      if ( nodes[i].css != -1 ) continue; // part of some chain
      if ( visited[i] ) continue;
      dfs_css(i, scc);
      scc++;
    }
#ifdef TIME
  printf("scc %d\n", scc);
#endif
    // 4) fix chains left & right edges
    for ( auto &c: cond )
    {
      if ( c.chain )
      {
        if ( c.left != -1 ) c.left = nodes[ c.left ].css;
        if ( c.right != -1 ) c.right = nodes[ c.right ].css;
#ifdef DEBUG
 printf("left %d right %d\n", c.left, c.right);
#endif
      }
    }
    // 5) make condensed graph
    cond.resize(scc);
    for ( int i = 0; i < N; ++i )
    {
      int id = nodes[i].css;
#ifdef DEBUG
 printf("node %d in %d order %d %c\n", i+1, id, nodes[i].chain_order, nodes[i].cutpoint ? 'Y' : ' ');
#endif
      for ( int e: nodes[i].edges )
      {
        if ( nodes[e].css != id ) cond[id].edges.insert(nodes[e].css);
      }
    }
    // and finally check if condensed graph is fully connected
    reset_cond();
    int cond_scc = 1;
    for ( int i = 0; i < scc; ++i )
    {
      if ( !visited[i] ) { dfs_cond(i, cond_scc); cond_scc++; }
    }
    cond_connected = (cond_scc == 2);
#ifdef TIME
    printf("cond_scc %d, cond_connected %d\n", cond_scc, cond_connected);
printTime("condensate");
#endif
  }
  void dfs_cond(int v, int sc_idx)
  {
    visited[v] = 1;
    cond[v].cond_scc = sc_idx;
    for ( int to: cond[v].edges )
    {
      if ( !visited[to] ) dfs_cond(to, sc_idx);
    }
  }
  void wave(int v, int t, int f)
  {
#ifdef DEBUG
 printf("wave %d\n", v);
#endif
    visited[v] = 1;
    wave_cnt++;
    if ( v == t ) return;
    for ( int e: cond[v].edges )
    {
      if ( e == f ) continue;
      if ( visited[e] ) continue;
      wave(e, t, f);
    }
  }
  int check(int a, int b, int c)
  {
    if ( a == b && b == c ) return 0;
    if ( a == c || b == c ) return 0;
    if ( a == b ) return 1;
    int id_a = nodes[a-1].css,
        id_b = nodes[b-1].css,
        id_c = nodes[c-1].css;
    if ( id_a == id_b && id_a == id_c && cond[id_a].chain )
    {
      // all 3 cities in one chain, A & B must be both on left or right side of C
      if ( nodes[a-1].chain_order < nodes[c-1].chain_order && nodes[b-1].chain_order < nodes[c-1].chain_order ) return 1;
      if ( nodes[a-1].chain_order > nodes[c-1].chain_order && nodes[b-1].chain_order > nodes[c-1].chain_order ) return 1;
      // C located between A & B
      return 0;
    }
    if ( id_a == id_b ) return 1;
    int ign = -1;
    if ( cond[id_c].chain )
    {
      ign = id_c;
      if ( id_a == id_c )
      {
        // if order A < order C then ignore right edge
        if ( nodes[a-1].chain_order < nodes[c-1].chain_order ) ign = cond[id_c].right;
        // if order A > order C then ignore left edge
        if ( nodes[a-1].chain_order > nodes[c-1].chain_order ) ign = cond[id_c].left;
      }
      // and the same if B & C belongs to the same chain
      if ( id_b == id_c )
      {
        if ( nodes[b-1].chain_order < nodes[c-1].chain_order ) ign = cond[id_c].right;
        if ( nodes[b-1].chain_order > nodes[c-1].chain_order ) ign = cond[id_c].left;
      }
#ifdef DEBUG
 printf("C %d in chain, ign %d\n", id_c, ign);
#endif
    } else {
      if ( nodes[c-1].cutpoint )
        ign = id_c;
      else {
        // if C is not cutpoint and not in chain - we have two variants
        // 1) if condensed graph is connected then A & B connected too
        // 2) else they are connected if located in the same SCC of condensed graph
        if ( cond_connected ) return 1;
        return ( cond[id_a].cond_scc == cond[id_b].cond_scc );
      }
    }
    if ( ign == id_a || ign == id_b ) return 0;
    reset_cond();
    wave(id_a, id_b, ign);
#ifdef DEBUG
printf("%d %d %d wvve %d ign %d\n", id_a, id_b, id_c, wave_cnt, ign);
#endif
    return visited[id_b];
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m, q;
  cin>>n>>m>>q;
  graph g(n);
  for ( int i = 0; i < m; ++i )
  {
    int a, b;
    cin>>a>>b;
    g.add_edge(a, b);
  }
  g.condense();
  for ( int i = 0; i < q; i++ )
  {
    int a, b, c;
    cin>>a>>b>>c;
    if ( g.check(a, b, c) ) puts("YES");
    else puts("NO");
  }
printTime("end");
}