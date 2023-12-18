#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;

// solution for Prufer Code
// details of algo: https://habr.com/ru/articles/331836/
void prufer_decode_linear (const vector<int> & prufer_code)
{
    int n = (int) prufer_code.size() + 2;
    vector<int> degree (n, 1);
    for (int i=0; i<n-2; ++i)
	++degree[prufer_code[i]];
 
    int ptr = 0;
    while (ptr < n && degree[ptr] != 1)
	++ptr;
    int leaf = ptr;
 
    vector < pair<int,int> > result;
    for (int i=0; i<n-2; ++i) {
	int v = prufer_code[i];
	result.push_back (make_pair (leaf, v));
 
	--degree[leaf];
	if (--degree[v] == 1 && v < ptr)
	    leaf = v;
	else {
	    ++ptr;
	    while (ptr < n && degree[ptr] != 1)
		++ptr;
	    leaf = ptr;
	}
    }
    for (int v=0; v<n-1; ++v)
	if (degree[v] == 1)
	    result.push_back (make_pair (v, n-1));
    // dump
    for ( auto &e: result )
      printf("%d %d\n", 1+e.first, 1+e.second);
}

int main()
{
  int n;
  cin>>n;
  vector<int> pcode(n-2);
  for ( int i = 2; i < n; i++ ) { cin>>pcode[i-2]; pcode[i-2]--; }
  prufer_decode_linear(pcode);
}
