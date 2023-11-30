#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>

// solution for Tree Traversals
// see algo in book "Binary Tree Problems: Must for Interviews and Competitive Coding", pages 41-48
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

struct node {
  int val;
  int left = -1, right = -1;
};

struct rt {
 unordered_map<int, int> order;
 vector<int> pre;
 vector<int> in;
 vector<node> rec;
 int cidx = -1;
 int preIndex = 0;
 rt(int n)
  : pre(n), in(n), rec(n)
 {}
 void read(int n)
 {
   // first read preorder
   for ( int i = 0; i < n; ++i ) cin>>pre[i];
   // then inorder and store indexes in order
   for ( int i = 0; i < n; ++i )
   {
     cin>>in[i];
     order[in[i]] = i;
   }
 }
 int make_tree(int istart, int iend)
 {
   if ( istart > iend ) return -1;
   int curr = pre[preIndex++];
   int res = ++cidx;
   node &n = rec[res];
   n.val = curr;
   if ( istart == iend ) return res;
   int inIndex = order[curr];
   n.left = make_tree(istart, inIndex - 1);
   n.right = make_tree(inIndex + 1, iend);
   return res;
 }
 void recover()
 {
   make_tree(0, (int)pre.size() - 1);
 }
 void dump_post_rec(int i)
 {
   if ( rec[i].left != -1 ) dump_post_rec(rec[i].left);
   if ( rec[i].right != -1 ) dump_post_rec(rec[i].right);
   printf("%d ", rec[i].val);
 }
 void dump_post()
 {
   dump_post_rec(0);
   printf("\n");
 }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n;
  cin>>n;
  rt t(n);
  t.read(n);
printTime("read");
  t.recover();
printTime("rec");
  t.dump_post();
}