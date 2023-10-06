#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <queue>
#include <string.h>
#include <algorithm> // for sort

// Critical Cities

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
  int from = 0;
  int rank = 0;
  int tin, fup;
  unordered_set<int> edges;
  unordered_set<int> in_edges;
  inline int degree() const
  {
    return edges.size();
  }
  char main_route = 0;
};

struct graph
{
  int target;
  vector<node> nodes;
  char *visited, *lead;
  graph(int N)
   : nodes(N)
  {
    target = N - 1;
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
    visited = (char *)malloc(N);
    lead = (char *)malloc(N);
    memset(lead, 0, N);
    memset(visited, 0, N);
  }
  inline void add_edge(int a, int b)
  {
    if ( a == b )
      return;
    node &n = nodes[a-1];
    n.edges.insert(b-1);
    node &m = nodes[b-1];
    m.in_edges.insert(a-1);
  }
  int timer = 0;
  vector<node *> main_route;
  // result
  unordered_set<int> res;
  void wave(int u)
  {
    unordered_set<int> s[2];
    int curr = 0;
    int next = 1;
    s[curr].insert(u);
    visited[u] = 1;
    while ( !s[curr].empty() )
    {
      for ( int u : s[curr] )
      {
        for ( int to: nodes[u].edges )
        {
          if ( to == u ) // cycle to itself
            continue;
          if ( !lead[to] )
            continue;
          if ( visited[to] )
            continue;
          visited[to] = 1;
          nodes[to].from = u;
// printf("u %d to %d\n", u + 1, to + 1);
          if ( to == target )
            return;
          s[next].insert(to);
        }
      }
      s[curr].clear();
      swap(curr, next);
    }
  }
  void wave2(int u)
  {
    queue<node *> s;
    int curr_rank = nodes[u].rank;
    int prev = -1;
    s.push(&nodes[u]);
    visited[u] = 1;
    while ( !s.empty() )
    {
      auto u = s.front(); s.pop();
      for ( int to: u->edges )
      {
          if ( to == u->n ) // cycle to itself
            continue;
          if ( !lead[to] ) continue;  
#ifdef DEBUG
 printf("u %d to %d\n", u->n + 1, to + 1);
#endif
          if ( visited[to] ) continue;
          // if ( nodes[to].main_route && nodes[to].rank < curr_rank ) continue;
          visited[to] = 1;
          if ( to == target ) return;
          if ( !nodes[to].main_route )
            s.push(&nodes[to]);
          else {
            if ( nodes[to].rank > curr_rank )
            {
              curr_rank = nodes[to].rank;
              if ( prev != -1 )
                s.push(&nodes[prev]);
              prev = nodes[to].n;
            } else
              s.push(&nodes[to]);
          }
      }
    }
  }
  void reset()
  {
    memset(visited, 0, target + 1);
  }
// based on https://e-maxx.ru/algo/cutpoints
  // complexity O(V + E)
  void dfs2(int v, int p = -1)
  {
    visited[v] = 1;
    nodes[v].tin = nodes[v].fup = timer++;
    for ( int to: nodes[v].edges )
    {
      if (to == p) // cycle to itself
        continue;
      if (visited[to])
        nodes[v].fup = min(nodes[v].fup, nodes[to].tin);
      else if ( lead[v] )
      {
        dfs2(to, v);
        nodes[v].fup = min(nodes[v].fup, nodes[to].fup);
        if (nodes[to].fup >= nodes[v].tin && p != -1)
        {
          if ( nodes[v].main_route )
            res.insert(v);
        }
      }
    }
  }
  void sharn(int u)
  {
    reset();
    timer = 1;
    dfs2(u);
  }
  void crit(int u)
  {
    queue<node *> q;
    lead[target-1] = 1; visited[target-1] = 1;
    q.push(&nodes[target-1]);
    while(!q.empty())
    {
      auto e = q.front(); q.pop();
      for ( auto to: e->in_edges )
      {
        if ( visited[to] ) continue;
        visited[to] = 1;
        lead[to] = 1;
        q.push(&nodes[to]);
      }
    }
    reset();
printTime("propagate");
// printf("u %d %d\n", u, nodes[u].lead);
    if ( !lead[u] )
      return;
    wave(u);
printTime("wave");
#ifdef DEBUG
 printf("target %d from %d\n", target, nodes[target-1].from);
#endif
    for ( int i = target; ; i = nodes[i].from )
    {
 // printf("i %d from %d\n", i + 1, 1 + nodes[i].from);
      main_route.push_back(&nodes[i]);
      if ( !nodes[i].from )
        break;
    }
    main_route.push_back(&nodes[0]);
    reverse(main_route.begin(), main_route.end());
    timer = 1;
    for ( auto mr = main_route.begin(); mr != main_route.end(); ++mr )
    {
      (*mr)->main_route = 1;
      (*mr)->rank = timer++;
    }
printTime("main_route");
    auto top = main_route.begin();
    while ( top != main_route.end() )
    {
      for ( ; top != main_route.end(); ++top )
       if ( (*top)->degree() == 1 )
        res.insert((*top)->n);
       else
        break;
      if ( top == main_route.end() || (*top)->n == target )
        break;
      reset();
#ifdef DEBUG
 printf("wave for %d: ", 1 + (*top)->n);
#endif
      wave2((*top)->n);
      res.insert((*top)->n);
      ++top;
      for ( auto br = top; br != main_route.end(); ++br )
      {
        if ( visited[(*br)->n] )
        {
#ifdef DEBUG
 printf("skip %d ", (*br)->n + 1);
#endif
          top = br;
        }
      }
      // put in res last reached by previous wave node
      if ( top != main_route.end() )
        res.insert((*top)->n);
#ifdef DEBUG
 printf("\n"); if ( top != main_route.end() ) printf("push last %d degree %d\n", (*top)->n + 1, (*top)->degree());
#endif
    }
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
  g.crit(0);
#ifdef DEBUG
  for ( auto mr: g.main_route )
    printf("mr %d %d\n", mr->n + 1, mr->rank);
#endif
printTime("end");
  g.res.insert(0);
  g.res.insert(g.nodes[n - 1].n);
  printf("%ld\n", g.res.size());
  vector<int> inc_res;
  std::copy(g.res.begin(), g.res.end(), std::back_inserter(inc_res));
  sort(inc_res.begin(), inc_res.end());
  for ( int r: inc_res )
    printf("%d ", r + 1);
}