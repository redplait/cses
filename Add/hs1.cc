#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <vector>

// greedy (and wrong) solution for Houses and Schools
// some variaton of K-clustering algo
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

struct item
{
  int64_t v;
  int idx;
  int c_idx = -1; // -1 means that element is not in cluster
  int left, size, centr_idx;
};

int main()
{
  int n, k;
  cin>>n>>k;
  if ( n == k )
  {
    puts("0"); return 0;
  }
  vector<item> h(n);
  for ( int i = 0; i < n; i++ )
  {
    h[i].idx = i;
    cin>>h[i].v;
  }
  int c_idx = 0;
  int curr_k = n;
  for( ; curr_k != k; curr_k-- )
  {
    int64_t best = LONG_MAX;
    int best_i;
    for ( int i = 0; i < n - 1; i++ )
    {
      // we have 4 cases - when both current and right not in cluster
      if ( -1 == h[i].c_idx && -1 == h[i+1].c_idx )
      {
        int64_t v = min(h[i].v, h[i+1].v);
        if ( v < best ) { best = v; best_i = i; }
        continue;
      }
      // 2) left not in cluster and right in cluster
      if ( -1 == h[i].c_idx && -1 != h[i+1].c_idx )
      {
        int64_t v = abs(i - h[i+1].centr_idx) * min(h[i].v, h[h[i+1].centr_idx].v);
        if ( v < best ) { best = v; best_i = i; }
        continue;
      }
      // 3) left in cluster and right not
      if ( -1 != h[i].c_idx && -1 == h[i+1].c_idx )
      {
        int64_t v = abs(i + 1 - h[i].centr_idx) * min(h[i+1].v, h[h[i].centr_idx].v);
        if ( v < best ) { best = v; best_i = i; }
        continue;
      }
      // final case - both in clusters
      if ( h[i].c_idx == h[i+1].c_idx ) continue;
      int64_t v = abs(h[i].centr_idx - h[i+1].centr_idx) * min(h[h[i].centr_idx].v, h[h[i+1].centr_idx].v);
      if ( v < best ) { best = v; best_i = i; }
    }
#ifdef DEBUG
 printf("best %d, l_idx %d r_idx %d, best %ld\n", best_i, h[best_i].c_idx, h[best_i+1].c_idx, best);
#endif
    // merge best_i & best_i + 1 into cluster
    // we again have 4 cases here
    if ( -1 == h[best_i].c_idx && -1 == h[best_i+1].c_idx )
    {
      h[best_i].c_idx = h[best_i+1].c_idx = c_idx++;
      h[best_i].size = 2;
      h[best_i].left = best_i;
      if ( h[best_i].v < h[best_i+1].v )
         h[best_i].centr_idx = h[best_i+1].centr_idx = best_i + 1;
      else
         h[best_i].centr_idx = h[best_i+1].centr_idx = best_i;
      continue;
    }
    // left not in cluster - connect it to cluster at right
    if ( -1 == h[best_i].c_idx )
    {
      h[best_i].c_idx = h[best_i+1].c_idx;
      h[best_i].size = h[best_i+1].size + 1;
      h[best_i].left = best_i;
      // find new centroid
      double sum = 0.0, prod = 0.0;
      for ( int i = 0; i < h[best_i].size; i++ )
      {
        sum += h[best_i+i].v;
        prod += h[best_i+i].v * i;
      }
      int new_c = round(prod / sum) + best_i;
      for ( int i = 0; i < h[best_i].size; i++ ) h[best_i+i].centr_idx = new_c;
      continue;
    }
    // right not in cluster
    if ( -1 == h[best_i+1].c_idx )
    {
      // find leftmost node for this cluster
      int left;
      for ( int i = best_i; i >= 0; i-- )
        if ( h[i].c_idx != h[best_i].c_idx ) break;
        else left = i;
      h[best_i+1].c_idx = h[left].c_idx;
      h[left].size++;
      // find new centroid
      double sum = 0.0, prod = 0.0;
      for ( int i = 0; i < h[left].size; i++ )
      {
        sum += h[left+i].v;
        prod += h[left+i].v * i;
      }
      int new_c = round(prod / sum) + left;
      for ( int i = 0; i < h[left].size; i++ ) h[left+i].centr_idx = new_c;
      continue;
    }
    // merge two adjacent clusters
    int left;
    for ( int i = best_i; i >= 0; i-- )
      if ( h[i].c_idx != h[best_i].c_idx ) break;
      else left = i;
    for ( int i = 0; i < h[best_i+1].size; i++ )
    {
      h[best_i+1+i].c_idx = h[left].c_idx;
      h[left].size++;
    }
    // find new centroid
    double sum = 0.0, prod = 0.0;
    for ( int i = 0; i < h[left].size; i++ )
    {
       sum += h[left+i].v;
       prod += h[left+i].v * i;
    }
    int new_c = round(prod / sum) + left;
    for ( int i = 0; i < h[left].size; i++ ) h[left+i].centr_idx = new_c;
  }
  // dump
  int64_t diff = 0;
  for ( int i = 0; i < n; )
  {
    if ( h[i].c_idx == -1 )
    {
#ifdef DEBUG
      printf("%d\n", i);
#endif
      i++;
    } else {
      // calc diff
      for ( int j = 0; j < h[i].size; j++ )
      {
        if ( i + j == h[i].centr_idx ) continue;
        diff += abs(j + i - h[i].centr_idx) * h[i+j].v;
      }
#ifdef DEBUG
      printf("c %d left %d size %d centr %d diff %ld\n", h[i].c_idx, h[i].left, h[i].size, h[i].centr_idx, diff);
#endif
      i += h[i].size;
    }
  }
  printf("%ld\n", diff);
printTime("res");
};