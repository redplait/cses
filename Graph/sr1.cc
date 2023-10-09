#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <limits.h>
#include <string.h>
#include <set>
#include <algorithm>

// Shortest Routes I

using namespace std;

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
    if (l < heap->count && heap->cmp(heap->data[l], heap->data[min]) < 0)
    {
        min = l;
    }
    if (r < heap->count && heap->cmp(heap->data[r], heap->data[min]) < 0)
    {
        min = r;
    }
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
  set<pair<int, int> > edges;
  inline int degree() const
  {
    return edges.size();
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

struct graph
{
  vector<node> nodes;
  vector<uint64_t> dist;
  vector<node *> parents;
  int edge_count = 0;
  char *visited;
  graph(int N)
   : nodes(N), dist(N), parents(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
    visited = (char *)malloc(N);
    fill(dist.begin(), dist.end(), ULONG_MAX);
    reset();
  }
  void reset()
  {
    memset(visited, 0, nodes.size());
  }
  inline void add_edge(int a, int b, int k)
  {
    if ( a == b )
      return;
    node &n = nodes[a-1];
    n.edges.insert( { b-1, k } );
    edge_count++;
//    node &m = nodes[b-1];
//    m.edges.push_back(a-1);
  }
  struct HeapNode
    {
      uint64_t dist;
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
    heap_insert(heap, HeapNode{ 0, s });
    while (heap->count > 0)
    {
        HeapNode head = heap_remove_root(heap);
        int u = head.u;
        visited[u] = true;
        for (auto &e: nodes[u].edges )
        {
            int to = e.first;
            if ( to == u ) continue;
            int w = e.second;
            if (!visited[to] && dist[u] + w < dist[to])
            {
                dist[to] = dist[u] + w;
                parents[to] = &nodes[u];
                heap_insert(heap, HeapNode{ dist[to], to});
            }
        }
     }
     free_heap(heap);
   }
   // version for sparsed graphs
   // based on set version from https://e-maxx.ru/algo/dijkstra_sparse
   void make_dijkstra2(int s)
   {
     set < pair<uint64_t,int> > q;
     dist[s] = 0;
     q.insert (make_pair (dist[s], s));
     while (!q.empty())
     {
	int v = q.begin()->second;
	q.erase (q.begin());
 
	for (auto &e: nodes[v].edges) {
	    int to = e.first,
		len = e.second;
	    if (dist[v] + len < dist[to]) {
		pair<uint64_t,int> p{ dist[to], to };
		q.erase ( p );
		p.first = dist[to] = dist[v] + len;
		q.insert(p);
	    }
	}
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
    int a, b, k;
    cin>>a>>b>>k;
    g.add_edge(a, b, k);
  }
printTime("filled");
  g.make_dijkstra2(0);
printTime("dijkstra");
  // shortest path
  for ( int i = 0; i < n; ++i )
  {
    if ( !i )
      printf("0 ");
    else
      printf("%ld ", g.dist[i]);
  }
}