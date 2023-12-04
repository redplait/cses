#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

// Sum of Three Values
// ripped from https://www.geeksforgeeks.org/find-a-triplet-that-sum-to-a-given-value/
using namespace std;

bool find3Numbers(vector<pair<int, int> > &A, int sum)
{
    int l, r;
    /* Sort the elements */
    sort(A.begin(), A.end(), [](const pair<int, int> &a, const pair<int, int> &b) { return a.first < b.first; });
    /* Now fix the first element one by one and find the
       other two elements */
    for (int i = 0; i < (int)A.size() - 2; i++) {
        // To find the other two elements, start two index
        // variables from two corners of the array and move
        // them toward each other
        l = i + 1; // index of the first element in the
        // remaining elements
        r = (int)A.size() - 1; // index of the last element
        while (l < r) {
            if (A[i].first + A[l].first + A[r].first == sum) {
                printf("%d %d %d", A[i].second, A[l].second, A[r].second);
                return true;
            }
            else if (A[i].first + A[l].first + A[r].first < sum)
                l++;
            else // A[i] + A[l] + A[r] > sum
                r--;
        }
    }
    // If we reach here, then no triplet was found
    return false;
}

int main()
{
  int n, x;
  cin>>n>>x;
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  vector<pair<int, int> > a(n);
  for ( int i = 0; i < n; ++i ) { cin>>a[i].first; a[i].second = 1+i; };
  if ( !find3Numbers(a, x) ) puts("IMPOSSIBLE");
}