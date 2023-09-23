#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <set>

// Mail Delivery
// actially we need to build Eulerian circuit
// but first to check if all nodes has even degrees and connected

using namespace std;

struct node
{
  int n;
  set<int> edges;
  bool visited = false;
  int degree() const
  {
    return (int)edges.size();
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
  graph(int N)
   : nodes(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i; 
  }
  inline void add_edge(int a, int b)
  {
    node &n = nodes[a-1];
    n.edges.insert(b-1);
    node &m = nodes[b-1];
    m.edges.insert(a-1);
  }
  void del_edge(int a, int b)
  {
    node &n = nodes[a];
    n.edges.erase(b);
    node &m = nodes[b];
    m.edges.erase(a);
  }
  void dfs(int i)
  {
    stack<node *> st;
    st.push(&nodes[i]);
    nodes[i].visited = true;
    while( !st.empty() )
    {
      auto c = st.top(); st.pop();
      for ( int ci: c->edges )
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
    for ( const node &n: nodes )
    {
      if ( !n.visited && n.degree() )
      {
#ifdef REASON
 printf("not visited %d\n", n.n);
#endif
        return 0;
      }
      auto size = n.edges.size();
      if ( size & 1 )
      {
#ifdef REASON
 printf("odd degree on %d\n", n.n);
#endif
        return 0;
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
  g.dfs(0);
  if ( !g.has_eulerian() )
  {
    printf("IMPOSSIBLE");
    return 0;
  }
  g.make_cycle();
}