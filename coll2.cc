#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>

// Collecting Numbers II
// very cool task bcs it has huge amount of details to care about

using namespace std;

void dump_array(const char *pfx, int n, vector<int> &v)
{
  printf("%s:", pfx);
  for ( int i = 0; i < n; ++i )
    printf(" %d", v[i]);
  printf("\n");
}

int calc_pass_count(int n, map<int, int> &indexes)
{
  int pass_count = 0;
  int curr_max = 1;
  while(curr_max <= n)
  {
    pass_count++;
    // lets check as much sequential values > curr_max in right part as we can
    int pos = indexes[curr_max];
    curr_max++;
    while(curr_max <= n)
    {
      int pos_next = indexes[curr_max];
      if ( pos_next < pos )
        break;
      pos = pos_next;
      curr_max++;
    }
  }
  return pass_count;
}

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);   //I/O fast
  int n, m;
  cin>>n; cin>>m;
  // read numbers and precalc positions: key is number, value is position where this number is
  vector<int> v(n);
  map<int, int> indexes;
  for ( int i = 0; i < n; ++i )
  {
    cin>>v[i];
    indexes[v[i]] = i;
  }
  if ( 1 == n )
  {
    printf("1\n");
    return 0;
  }
  // calc initial pass_count
  int pass_count = calc_pass_count(n, indexes);
#ifdef DEBUG
 printf("initial pass_count %d\n", pass_count);
 dump_array("old v", n, v);
#endif
  // read swaps indexes
  for ( int s = 0; s < m; ++s )
  {
    // read current swap op
    int s1, s2;
    cin>>s1;
    cin>>s2;
    // trivial case - s1 eq s2, no changes
    if ( s1 == s2 )
    {
      printf("%d\n", pass_count);
      continue;
    }
    // c++ indexes start from 0
    int need_recalc = 0;
    s1--; s2--;
    int old1 = v[s1];
    int old2 = v[s2];
#ifdef DEBUG
 printf("s1 %d old1 %d s2 %d old2 %d\n", s1, s2, old1, old2);
#endif
    auto i1 = indexes.find(old1);
    auto i2 = indexes.find(old2);
    // how can change pass_count if old1 number now in position s2?
    // there are several (in)variants: let I is positiion of next element
    //  if old position was < I and s2 is > I then pass_count++
    //  if old position was > I and s2 < I then pass_count--
    // check next pos for old1
    if ( old1 != n )
    {
      auto old_next_pos = indexes[old1 + 1];
      auto next_pos = (old1 + 1 == old2) ? s1 : old_next_pos;
      if ( i1->second < old_next_pos && s2 > next_pos )
      {
#ifdef DEBUG
        printf("old1_next %d was at %d and now at %d > %d inc\n", old1, i1->second, s2, next_pos);
#endif
        pass_count++;
      } else if ( i1->second > old_next_pos && s2 < next_pos )
      {
#ifdef DEBUG
        printf("old1_next %d was at %d and now at %d < %d dec\n", old1, i1->second, s2, next_pos);
#endif
        pass_count--;
      }
    }
    // check prev pos for old1
    // let I is positiion of prev element
    //  if old position was > I and s2 > I then pass_count--
    //  if old position was < I and s2 < I then pass_count++
    if ( old1 != 1 )
    {
      auto old_prev_pos = indexes[old1 - 1];
      auto prev_pos = (old1 - 1 == old2) ? s1 : old_prev_pos;
      if ( old_prev_pos > i1->second && s2 > prev_pos )
      {
#ifdef DEBUG
        printf("old1_prev %d was at %d and now at %d < %d dec\n", old1, i1->second, s2, prev_pos);
#endif
        pass_count--;
      } else if ( old_prev_pos < i1->second && s2 < prev_pos )
      {
#ifdef DEBUG
        printf("old1_prev %d was at %d and now at %d > %d inc\n", old1, i1->second, s2, prev_pos);
#endif
        pass_count++;
      }
    }
    // the same for old2 & s1
    if ( old2 != n && old2 + 1 != old1 )
    {
      auto old_next_pos = indexes[old2 + 1];
      auto next_pos = (old2 + 1 == old1) ? s2 : old_next_pos;
      if ( i2->second < old_next_pos && s1 > next_pos )
      {
#ifdef DEBUG
        printf("old2_next %d was at %d and now at %d > %d inc\n", old2, i2->second, s1, next_pos);
#endif
        pass_count++;
      } else if ( i2->second > old_next_pos && s1 < next_pos )
      {
#ifdef DEBUG
        printf("old2_next %d was at %d and now at %d < %d dec\n", old2, i2->second, s1, next_pos);
#endif
        pass_count--;
      }
    }
    // check prev pos for old2
    if ( old2 != 1 && old2 - 1 != old1 )
    {
      auto old_prev_pos = indexes[old2 - 1];
      auto prev_pos = (old2 - 1 == old1) ? s2 : old_prev_pos;
      if ( old_prev_pos > i2->second && s1 > prev_pos )
      {
#ifdef DEBUG
        printf("old2_prev %d was at %d and now at %d > %d dec\n", old2, i2->second, s1, prev_pos);
#endif
        pass_count--;
      } else if ( old_prev_pos < i2->second && s1 < prev_pos )
      {
#ifdef DEBUG
        printf("old2_prev %d was at %d and now at %d < %d inc\n", old2, i2->second, s1, prev_pos);
#endif
        pass_count++;
      }
    }
    // update indexes
    swap(v[s2], v[s1]);
    // update precalc indexes
    int tmp_pos = i1->second;
    i1->second = i2->second;
    i2->second = tmp_pos;
    if ( need_recalc )
      pass_count = calc_pass_count(n, indexes);
#ifdef DEBUG
    // dump new array
    dump_array("new v", n, v);
#endif
    printf("%d\n", pass_count);
  }
}