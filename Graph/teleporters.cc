#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <algorithm>

// solution for Teleporters Path
// we need to build Eulerian path: http://e-maxx.ru/algo/euler_path
// algo for traversing is the same for directed and undirected graphs
// but verification for if graph has eulerian path is totally different
// we need to check not degrees but difference between out-edges & in-edges
// if node is S then out-edges must be 1 more than in-edges - else you will be ubable to leave it
// if node is T then in-edges must be 1 more than out-edges - else you never reach it
// and all remained vertices must have the same degrees for in & out edges
// but first check if all nodes having edges are connected - see is_connected method

using namespace std;

struct node
{
  int n;
  set<int> edges;
  set<int> in_edges;
  bool visited = false;
  inline int degree() const
  {
    return (int)edges.size();
  }
  inline int in_degree() const
  {
    return (int)in_edges.size();
  }
  inline bool empty() const
  {
    return edges.empty() && in_edges.empty();
  }
  void dump() const
  {
    printf("%d: ", n);
    for ( int c: edges )
      printf("%d ", c);
    printf("\n");
  }
};

struct graph
{
  vector<node> nodes;
  int N;
  graph(int n)
   : nodes(n), N(n)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
  }
  inline void add_edge(int a, int b)
  {
    node &n = nodes[a-1];
    n.edges.insert(b-1);
    node &m = nodes[b-1];
    m.in_edges.insert(a-1);
  }
  void del_edge(int a, int b)
  {
    node &n = nodes[a];
    n.edges.erase(b);
    node &m = nodes[b];
    m.in_edges.erase(a);
  }
  void imposs()
  {
    puts("IMPOSSIBLE");
    exit(0);
  }
  int is_connected()
  {
    if ( !has_eulerian() ) imposs();
    for ( auto &n: nodes )
      if ( !n.visited && !n.empty() )
      {
#ifdef REASON
 printf("level %d is not visited\n", 1+n.n);
#endif
        imposs();
      }
    return 1;
  }
  void dfs(int i)
  {
    stack<node *> st;
    st.push(&nodes[i]);
    nodes[i].visited = true;
    while( !st.empty() )
    {
      auto c = st.top(); st.pop();
      for ( int ci: c->in_edges )
      {
        if ( nodes[ci].visited )
          continue;
#ifdef DEBUG
 printf("%d -> %d\n", c->n, ci);
#endif
        nodes[ci].visited = true;
        st.push(&nodes[ci]);
      }
    }
  }
  int has_eulerian() const
  {
    if ( !nodes[0].visited || !nodes[N-1].visited ) return 0;
    for ( const node &n: nodes )
    {
      if ( !n.visited ) continue;
      if ( n.n == 0 ) // S
      {
        if ( n.degree() - 1 != n.in_degree() )
        {
#ifdef REASON
 printf("S degree %ld, in-degree %ld\n", n.degree(), n.in_degree());
#endif
           return 0;
        }
      } else if ( n.n == N-1) // T
      {
        if ( n.degree() != n.in_degree() - 1 )
        {
#ifdef REASON
 printf("T degree %ld, in-degree %ld\n", n.degree(), n.in_degree());
#endif
          return 0;
        }
      } else if ( !n.empty() )
      {
        if ( n.degree() != n.in_degree() )
        {
#ifdef REASON
 printf("vertex %d degree %ld, in-degree %ld\n", 1+n.n, n.degree(), n.in_degree());
#endif
          return 0;
        }
      }
    }
    return 1;
  }
  void make_cycle()
  {
    vector<int> res;
    stack<node *>st;
    // put initial node
    st.push(&nodes[0]);
    while(!st.empty())
    {
      auto n = st.top();
#ifdef DEBUG
 n->dump();
#endif
      if ( !n->degree() )
      {
#ifdef REASON
 printf("dead-end on %d\n", 1+n->n);
#endif
        res.push_back(n->n);
        st.pop();
        continue;
      }
      // get first unvisited edge in n node
      auto u = *n->edges.begin();
      del_edge(n->n, u);
#ifdef DEBUG
 printf("u %d -", u); nodes[u].dump();
#endif
      st.push(&nodes[u]);
    }
    if ( !res.empty() && res[0] != N - 1 ) imposs();
    // check if we have some remained edges
    for ( auto &n: nodes )
    {
      if ( n.degree() )
      {
#ifdef REASON
 printf("node %d still has degree %d\n", 1+n.n, n.degree()); n.dump();
#endif
        imposs();
      }
    }
    reverse(res.begin(), res.end());
    for ( int r: res )
      printf("%d ", r + 1);
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  graph g(n);
  for ( int i = 0; i < m; ++i )
  {
    int a, b;
    cin>>a>>b;
    g.add_edge(a, b);
  }
  g.dfs(n-1);
  g.is_connected();
  g.make_cycle();
}