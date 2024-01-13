#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <unordered_map>

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
  int c_idx = -1; // -1 means that element is not in cluster
};

struct cluster
{
  int left, size, centr_idx;
  int64_t cf;
  bool has_left = false, has_right = false;
  int64_t l_cf, r_cf;
  int centr_l, centr_r;
};

struct hs_cluster
{
  int n, k, curr_k, c_idx = 0;
  vector<item> h;
  vector<cluster> cm;
  hs_cluster(int N, int K): n(N), k(K), h(N)
  {}
 void find_centroid(cluster &c, int idx)
 {
  if ( 1 == c.size )
  {
    c.centr_idx = idx;
    return;
  }
  double sum = 0.0, prod = 0.0;
  for ( int i = 0; i < c.size; i++ )
  {
    sum += h[idx+i].v;
    if (i) prod += h[idx+i].v * i;
  }
  c.centr_idx = round(prod / sum) + idx;
 }
 void update_left(cluster &c)
 {
  double sum = 0.0, prod = 0.0;
  for ( int i = 0; i < c.size + 1; i++ )
  {
    sum += h[c.left+i-1].v;
    if (i) prod += h[c.left+i-1].v * i;
  }
  c.centr_l = round(prod / sum) + c.left - 1;
  c.l_cf = 0;
  for ( int i = 0; i < c.size + 1; i++ )
  {
    c.l_cf += abs(i + c.left - 1 - c.centr_l) * h[c.left+i-1].v;
  }
 }
 void update_right(cluster &c)
 {
  double sum = 0.0, prod = 0.0;
  for ( int i = 0; i < c.size + 1; i++ )
  {
    sum += h[c.left+i].v;
    if (i) prod += h[c.left+i].v * i;
  }
  c.centr_r = round(prod / sum) + c.left;
  c.r_cf = 0;
  for ( int i = 0; i < c.size + 1; i++ )
  {
    c.r_cf += abs(i + c.left - c.centr_r) * h[c.left+i].v;
  }
 }
 void init(cluster &c)
 {
   c.has_left = c.has_right = false;
   if ( c.left > 1 && h[c.left - 1].c_idx == -1 ) c.has_left = true;
   if ( c.left + c.size < n - 1 && h[c.left + c.size].c_idx == -1 ) c.has_right = true;
   if ( c.has_left ) update_left(c);
   if ( c.has_right ) update_right(c);
 }
 void debug_dump()
 {
  for ( int i = 0; i < (int)h.size(); i++ )
  {
    printf("[%d] %ld", i, h[i].v);
    if ( h[i].c_idx == -1 )
    {
      printf("\n");
      continue;
    }
    const cluster &c = cm[h[i].c_idx];
    printf(" c_idx %d left %d size %d centr %d\n", h[i].c_idx, c.left, c.size, c.centr_idx);
  }
 }
 int64_t est_adj(const vector<item> &h, int idx, int &new_c)
 {
  cluster &lc = cm[h[idx].c_idx],
          &rc = cm[h[idx+1].c_idx];
  int left = lc.left;
  // calc new centroid
  double sum = 0.0, prod = 0.0;
  for ( int i = 0; i < lc.size + rc.size; i++ )
  {
    sum += h[left+i].v;
    if (i) prod += h[left+i].v * i;
  }
  new_c = round(prod / sum) + left;
  int64_t res = 0;
  for ( int i = 0; i < lc.size + rc.size; i++ )
  {
    res += abs(i + left - new_c) * h[i + left].v;
  }
  return res;
 }
 int calc2(bool no_merge)
 {
    int64_t v, best = LONG_MAX;
    int best_i = -1, curr_c, new_c;
    bool cut_l = false, cut_r = false;
    for ( int i = 0; i < n - 1; i++ )
    {
      // we have 4 cases - when both current and right not in cluster
      if ( -1 == h[i].c_idx && -1 == h[i+1].c_idx )
      {
        v = min(h[i].v, h[i+1].v);
        if ( v < best ) { best = v; best_i = i; cut_l = cut_r = false; }
        continue;
      }
      // 2) left not in cluster and right in cluster
      if ( -1 == h[i].c_idx && -1 != h[i+1].c_idx )
      {
        cluster &c = cm[h[i+1].c_idx];
        if ( c.l_cf < best ) { best = c.l_cf; best_i = i; cut_l = cut_r = false; }
        continue;
      }
      // 3) left in cluster and right not
      if ( -1 != h[i].c_idx && -1 == h[i+1].c_idx )
      {
        cluster &c = cm[h[i].c_idx];
        if ( c.r_cf < best ) { best = c.r_cf; best_i = i; cut_l = cut_r = false; }
        continue;
      }
      // final case - both in clusters
      if ( h[i].c_idx == h[i+1].c_idx ) continue;
      int64_t v_left, v_right;
      cluster &lc = cm[h[i].c_idx], &rc = cm[h[i+1].c_idx];
      // check if node[i] should be merged with right cluster
      v_left = (i - lc.centr_idx) * h[i].v;
      v_right = (rc.centr_idx - i) * h[i].v;
      if ( v_left > v_right )
      {
        v = v_left - v_right;
        if ( v < best ) { best = v; best_i = i; cut_l = true; cut_r = false; }
      }
      // check if node[i+1] should be merged with left cluster
      v_left = (i + 1 - lc.centr_idx) * h[i+1].v;
      v_right = (rc.centr_idx - i - 1) * h[i+1].v;
      if ( v_left < v_right )
      {
        v = v_right - v_left;
        if ( v < best ) { best = v; best_i = i; cut_r = true; cut_l = false; }
      }
      // finally check if we need to merge 2 adjacent cluster
      if ( !no_merge )
      {
        v = est_adj(h, i, curr_c);
        if ( v < best )
        {
          best = v;
          new_c = curr_c;
          best_i = i;
          cut_l = cut_r = false;
        }
      }
    }
    if ( -1 == best_i ) return 0;
#ifdef DEBUG
 if ( -1 == best_i ) { puts("BUG"); exit(-1); }
 printf("best_i %d, l_idx %d r_idx %d, best %ld", best_i, h[best_i].c_idx, h[best_i+1].c_idx, best);
 if ( cut_l ) printf(" cut_l");
 if ( cut_r ) printf(" cut_r");
 printf("\n");
#endif
    // we again have 4 cases here
    // 1) both nodes are not in clusters, merge best_i & best_i + 1 into new cluster
    if ( -1 == h[best_i].c_idx && -1 == h[best_i+1].c_idx )
    {
      cluster cc;
      cc.size = 2;
      cc.left = best_i;
      if ( h[best_i].v < h[best_i+1].v )
         cc.centr_idx = best_i + 1;
      else
         cc.centr_idx = best_i;
      init(cc);
      h[best_i].c_idx = h[best_i+1].c_idx = c_idx;
      cm.push_back(cc);
      c_idx++;
      return 1;
    }
    // 2) left not in cluster - connect it to cluster at right
    if ( -1 == h[best_i].c_idx )
    {
      cluster &cc = cm[h[best_i+1].c_idx];
      h[best_i].c_idx = h[best_i+1].c_idx;
      cc.size++;
      cc.left = best_i;
      cc.centr_idx = cc.centr_l;
      init(cc);
      return 1;
    }
    // 3) right not in cluster - connect it to cluster at left
    if ( -1 == h[best_i+1].c_idx )
    {
      cluster &cc = cm[h[best_i].c_idx];
      h[best_i+1].c_idx = h[best_i].c_idx;
      cc.size++;
      cc.centr_idx = cc.centr_r;
      init(cc);
      return 1;
    }
    // merge two adjacent clusters
    cluster &lc = cm[h[best_i].c_idx], &rc = cm[h[best_i+1].c_idx];
    if ( cut_l )
    {
#ifdef DEBUG
 printf("cut_l lc %d size %d left %d rc %d size %d\n", h[best_i].c_idx, lc.size, lc.left, h[best_i+1].c_idx, rc.size);
#endif
      lc.size--;
      if ( lc.size < 2 ) { h[best_i-1].c_idx = -1; }
      rc.size++;
      h[best_i].c_idx = h[best_i+1].c_idx;
      rc.left = best_i;
      if ( h[best_i-1].c_idx != -1 )
      {
        find_centroid(lc, lc.left);
        init(lc);
      }
      find_centroid(rc, rc.left);
      init(rc);
      curr_k++;
      return 1;
    }
    // connect node[best_i + 1] with cluster at left
    if ( cut_r )
    {
      rc.size--;
      if ( rc.size < 2 ) { h[best_i+2].c_idx = -1; }
      else rc.left++;
      lc.size++;
      h[best_i+1].c_idx = h[best_i].c_idx;
      if ( h[best_i+2].c_idx != -1 )
      {
        find_centroid(rc, rc.left);
        init(rc);
      }
      find_centroid(lc, lc.left);
      init(lc);
      curr_k++;
      return 1;
    }
    if ( no_merge ) return 0;
    // merge 2 adjacent cluster
#ifdef DEBUG
 printf("merge %d (size %d left %d) & %d (size %d left %d)\n", h[best_i].c_idx, lc.size, lc.left, h[best_i+1].c_idx, rc.size, rc.left);
#endif
    for ( int i = 0; i < rc.size; i++ )
    {
      h[best_i+1+i].c_idx = h[best_i].c_idx;
      lc.size++;
    }
#ifdef DEBUG
 printf("new centr_idx %d\n", new_c);
#endif
    lc.centr_idx = new_c;
    find_centroid(lc, lc.left);
    init(lc);
    return 2;
 }
 void mark(cluster &c, int idx)
 {
  for ( int i = 0; i < c.size; i++ ) h[c.left + i].c_idx = idx;
 }
 void calc()
 {
  int res;
  if ( k < (n / 3) )
  {
    res = 2;
    // divide into k equal parts
    int curr = 0, step = n / k;
    for ( int i = 0; i < k; i++ )
    {
      cluster c;
      c.left = curr;
      if ( i == k - 1 ) // for last put all remained nodes
        c.size = n - curr;
      else
        c.size = step;
      mark(c, c_idx);
      find_centroid(c, c.left);
      cm.push_back(c);
      c_idx++;
      curr += step;
    }
  } else
    for( curr_k = n; curr_k != k; curr_k-- ) res = calc2(false);
  if ( 2 == res ) while( calc2(true) );
  // dump
  int64_t diff = 0;
  for ( int i = 0; i < n; )
  {
    if ( h[i].c_idx == -1 )
    {
#ifdef DEBUG
      printf("%d - %d\n", i, h[i].v);
#endif
      i++;
    } else {
      // calc diff
      cluster &cc = cm[h[i].c_idx];
      for ( int j = cc.left; j < cc.left + cc.size; j++ )
      {
        if ( j == cc.centr_idx ) continue;
        diff += abs(j - cc.centr_idx) * h[j].v;
      }
#ifdef DEBUG
      printf("c %d left %d size %d centr %d diff %ld\n", h[i].c_idx, cc.left, cc.size, cc.centr_idx, diff);
#endif
      i += cc.size;
    }
  }
  printf("%ld\n", diff);
printTime("res");
 }
};

int main()
{
  int n, k;
  cin>>n>>k;
  if ( n == k )
  {
    puts("0"); return 0;
  }
  hs_cluster hs(n, k);
  for ( int i = 0; i < n; i++ ) cin>>hs.h[i].v;
  hs.calc();
};