#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

// Sum of Four Values
// ripped from https://www.geeksforgeeks.org/find-four-elements-that-sum-to-a-given-value-set-2/
using namespace std;

struct pairSum {
    // index (int A[]) of first element in pair
    int first;
    // index of second element in pair
    int sec;
    // sum of the pair
    int sum;
};

// Function to check if two given pairs
// have any common element or not
bool noCommon(pairSum &a, pairSum &b)
{
    if ( a.first == b.first || a.first == b.sec
        || a.sec == b.first || a.sec == b.sec)
        return false;
    return true;
}

// The function finds four
// elements with given sum X
bool find4Numbers(vector<int> &arr, int X)
{
    int i, j, n = (int)arr.size();
 
    // Create an auxiliary array
    // to store all pair sums
    int size = (n * (n - 1)) / 2;
    pairSum aux[size];
 
    // Generate all possible pairs
    // from A[] and store sums
    // of all possible pairs in aux[]
    int k = 0;
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            aux[k].sum = arr[i] + arr[j];
            aux[k].first = i + 1;
            aux[k].sec = j + 1;
            k++;
        }
    }
 
    // Sort the aux[] array
    sort(aux, aux + size, [](const pairSum &a, const pairSum &b) { return a.sum < b.sum; });
    // Now start two index variables
    // from two corners of array
    // and move them toward each other.
    i = 0;
    j = size - 1;
    while (i < size && j >= 0) {
        if ((aux[i].sum + aux[j].sum == X)
            && noCommon(aux[i], aux[j])) {
            printf("%d %d %d %d\n", aux[i].first, aux[i].sec, aux[j].first, aux[j].sec);
            return true;
        }
        else if (aux[i].sum + aux[j].sum < X)
            i++;
        else
            j--;
    }
    return false;
}

int main()
{
  int n, x;
  cin>>n>>x;
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  vector<int> a(n);
  for ( int i = 0; i < n; ++i ) cin>>a[i];
  if ( !find4Numbers(a, x) ) puts("IMPOSSIBLE");
}