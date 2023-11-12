#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <limits.h>
#include <string.h>
#include <queue>
#include <set>
#include <unordered_set>
#include <algorithm>

// Visiting Cities
// solution uses weird trick - we can build 2 shortest paths
// 1) from source to target
// 2) from target to source
// like in algo for graph connectivity testing
// But we can calculate how many paths there was for each vertex in such paths - direct & reverse count
// and then select from first shortest path only vertices with direct_cnt * reverse_cnt eq to direct count from target
// Sure this can be huge number so use modular operations with prime number as modulo suitable for int type
// like this one https://www.wolframalpha.com/input?i=105097564th+prime
const int64_t mod = 2147483629;
using namespace std;
// type to hold distances
typedef int64_t CT;

template<typename T>
struct Heap
{
    T *data;
    int count;
    int lim;
    int (*cmp)(T, T);
};

template<typename T>
void swap2(T *x, T *y)
{
    T tmp = *x;
    *x = *y;
    *y = tmp;
}

template<typename T>
Heap<T> *new_heap(int limit, int (*cmp)(T, T))
{
    Heap<T> *heap = new Heap<T>();
    heap->data = new T[limit];
    heap->count = 0; heap->lim = limit;
    heap->cmp = cmp;
    return heap;
}

template<typename T>
Heap<T> *new_heap(int limit)
{
    int (*cmp)(T, T) = [](T x, T y) -> int
    {
        return x - y;
    };
    return new_heap(limit, cmp);
}

template<typename T>
void free_heap(Heap<T> *heap)
{
    delete[] heap->data;
    delete heap;
}

template<typename T>
void heapify_up(Heap<T> *heap, int i)
{
    while (i > 0 && heap->cmp(heap->data[i], heap->data[(i - 1) / 2]) < 0)
    {
        swap2(&heap->data[i], &heap->data[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

template<typename T>
void heapify_down(Heap<T> *heap, int i)
{
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    int min = i;
    if (l < heap->count && heap->cmp(heap->data[l], heap->data[min]) < 0) min = l;
    if (r < heap->count && heap->cmp(heap->data[r], heap->data[min]) < 0) min = r;
    if (min != i)
    {
        swap2(&heap->data[i], &heap->data[min]);
        heapify_down(heap, min);
    }
}

template<typename T>
void heap_insert(Heap<T> *heap, T data)
{
    heap->data[heap->count++] = data;
    heapify_up(heap, heap->count - 1);
}

template<typename T>
T heap_root(Heap<T> *heap)
{
    return heap->data[0];
}

template<typename T>
T heap_remove_root(Heap<T> *heap)
{
    T min = heap->data[0];
    heap->data[0] = heap->data[--heap->count];
    heapify_down(heap, 0);
    return min;
}

struct node
{
  int n;
  int rank = 0;
  int64_t direct_cnt = 0;
  int64_t reverse_cnt = 0;
  // note that inside each node distance is just int
  set<pair<int, int> > edges;
  set<pair<int, int> > in_edges;
  inline int degree() const
  { return (int)edges.size(); }
  inline int in_degree() const
  { return (int)in_edges.size(); }
  inline void put(int v, int w)
  { edges.insert({ v, w }); }
  inline void in_put(int v, int w)
  { in_edges.insert({ v, w }); }
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

struct graph
{
  vector<node> nodes;
  // and here dist is CT bcs it is sum of many distances from source upto some vertex
  vector<CT> dist;
  vector<node *> parents;
  int edge_count = 0;
  char *visited;
  graph(int N)
   : nodes(N), dist(N), parents(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
    visited = (char *)malloc(N);
    reset();
  }
  void reset()
  {
    fill(dist.begin(), dist.end(), LONG_MAX);
    memset(visited, 0, nodes.size());
  }
  inline void add_edge(int a, int b, int k)
  {
    if ( a == b )
      return;
    node &n = nodes[a-1];
    n.put( b-1, k );
    edge_count++;
    node &m = nodes[b-1];
    m.in_put(a-1, k);
  }
  struct HeapNode
    {
      CT dist;
      int u;
    };
    int (*cmp)(HeapNode, HeapNode) = [](HeapNode x, HeapNode y) -> int
    {
        return x.dist - y.dist;
    };
  void make_dijkstra(int s)
  {
    Heap<HeapNode> *heap = new_heap(edge_count, cmp);
    dist[s] = 0;
    nodes[s].direct_cnt = 1;
    heap_insert(heap, HeapNode{ 0, s });
    while (heap->count > 0)
    {
        HeapNode head = heap_remove_root(heap);
        int u = head.u;
        auto d = head.dist;
        visited[u] = true;
        if ( d > dist[u] ) continue;
        for (auto &e: nodes[u].edges )
        {
            int to = e.first;
            if ( to == u ) continue;
            CT w = e.second;
            if ( !visited[to] && dist[u] + w < dist[to])
            {
                nodes[to].direct_cnt = nodes[u].direct_cnt;
                dist[to] = dist[u] + w;
                parents[to] = &nodes[u];
                heap_insert(heap, HeapNode{ dist[to], to});
            } else if ( dist[u] + w == dist[to] ) // we should ignore visited flag here
            {
              nodes[to].direct_cnt += nodes[u].direct_cnt;
              nodes[to].direct_cnt %= mod;
            }
        }
     }
     free_heap(heap);
   }
  void dijkstra_rev(int s)
  {
    Heap<HeapNode> *heap = new_heap(edge_count, cmp);
    dist[s] = 0;
    nodes[s].reverse_cnt = 1;
    heap_insert(heap, HeapNode{ 0, s });
    while (heap->count > 0)
    {
        HeapNode head = heap_remove_root(heap);
        int u = head.u;
        auto d = head.dist;
        visited[u] = true;
        if ( d > dist[u] ) continue;
        for (auto &e: nodes[u].in_edges )
        {
            int to = e.first;
            if ( to == u ) continue;
            CT w = e.second;
            if ( !visited[to] && dist[u] + w < dist[to])
            {
                nodes[to].reverse_cnt = nodes[u].reverse_cnt;
                dist[to] = dist[u] + w;
                // parents[to] = &nodes[u]; -- don`t need to save parents for second pass
                heap_insert(heap, HeapNode{ dist[to], to});
            } else if ( dist[u] + w == dist[to] )
            {
              nodes[to].reverse_cnt += nodes[u].reverse_cnt;
              nodes[to].reverse_cnt %= mod;
            }
        }
     }
     free_heap(heap);
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
    int a, b, k;
    cin>>a>>b>>k;
    g.add_edge(a, b, k);
  }
  g.make_dijkstra(0);
  if ( !g.visited[n-1] )
  {
    puts("0");
    return 0;
  }
printTime("dijkstra");
  // collect 1st shortest path
  vector<node *> short_res;
  node *cur = &g.nodes[n-1];
  while( cur->n )
  {
    short_res.push_back(cur);
    cur = g.parents[ cur->n ];
    if ( !cur )
     break;
  }
  short_res.push_back(&g.nodes[0]);
  reverse(short_res.begin(), short_res.end());
#ifdef DEBUG
  printf("short_res %ld\n", short_res.size());
#endif
  g.reset();
  g.dijkstra_rev(n-1);
printTime("dijkstra_rev");
  vector<int> res;
  int64_t must_be = g.nodes[0].reverse_cnt;
//  int64_t must_be2 = g.nodes[n-1].direct_cnt;
#ifdef DEBUG
printf("direct0 %ld rev0 %ld direct_last %ld, rev_last %ld\n", g.nodes[0].direct_cnt, g.nodes[0].reverse_cnt, g.nodes[n-1].direct_cnt, g.nodes[n-1].reverse_cnt);
#endif
  for ( auto e: short_res )
  {
    int64_t cur_cnt = e->direct_cnt * e->reverse_cnt % mod;
    if ( cur_cnt < 0 ) cur_cnt += mod;
#ifdef DEBUG
  printf("%d: %ld %ld %ld\n", e->n, e->direct_cnt, e->reverse_cnt, cur_cnt);  
#endif
    if ( must_be == cur_cnt ) res.push_back(e->n);
  }
printTime("checks");
  // dump results
  sort(res.begin(), res.end(), less<int>());
  printf("%ld\n", res.size());
  for ( auto e: res )
    printf("%d ", e + 1);
}