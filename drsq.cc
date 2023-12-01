#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>

// solution for Dynamic Range Sum Queries
using namespace std;

typedef int64_t item;

struct segtree
{
  int size;
  vector<item> tree;
  const item INITIAL = 0;

  void init(int n)
  {
     size = 1;
     while(size < n) size *= 2;
     tree.assign(size*2,INITIAL);
  }
  inline item merge(item a, item b)
  {
     return a+b;
  }
  void build(int node, int lx, int rx, vector<item>& a)
  {
     if (rx-lx == 1) {
        if (lx < (int) a.size())
            tree[node] = a[lx];
        return;
     }
     int m = (lx + rx) / 2;
     build(2*node+1, lx, m, a);
     build(2*node+2, m, rx, a);
     tree[node] = merge(tree[2*node+1], tree[2*node+2]);
  }
  void build(vector<item>& a) {
     build(0,0,size,a);
  }
  void set(int i, item a, int node, int l, int r)
  {
     if (r - l == 1) {
        tree[node] = a;
        return;
     }
     int m = (l+r) / 2;
     if (i < m)
        set(i,a,2*node+1,l,m);
     else
        set(i,a,2*node+2,m,r);
     tree[node] = merge(tree[2*node+1], tree[2*node+2]);
  }
  void set(int i, long long a) {
    set(i,a,0,0,size);
  }
  item get(int node, int lx, int rx, int L, int R)
  {
     if (L >= rx || R <= lx) return INITIAL;
     if (lx >= L && rx <= R) return tree[node];
     int m = (lx + rx) / 2;
     item s1 = get(2*node+1, lx, m, L, R);
     item s2 = get(2*node+2, m, rx, L, R);
     return merge(s1,s2);
  }
  item get(int L, int R) {
    return get(0,0,size,L,R);
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n,q;
  cin>>n >>q;
  vector<item> a(n);
  for (int i = 0; i < n; i++) cin>>a[i];
  segtree st;
  st.init(n);
  st.build(a);
  while(q--)
  {
    int op;
    cin>>op;
    if (op == 1)
    {
      int i;
      item x;
      cin>>i>>x;
      st.set(i-1,x);
    } else {
      int l,r;
      cin>>l>>r;
      item out = st.get(l-1, r);
      printf("%ld\n", out);
    }
  }
}