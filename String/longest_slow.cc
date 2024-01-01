#include<bits/stdc++.h>
using namespace std;

// slow solution for Repeating Substring
// complexity O(N ^ 2)
// based on https://www.geeksforgeeks.org/longest-repeating-and-non-overlapping-substring/
const int maxL = 1e5;
int LCSRe1[maxL+1];
int LCSRe2[maxL+1];
int *re1 = LCSRe1, // LCSRe[i]
    *re2 = LCSRe2; // LCSRe[i-1]

inline void printTime(const char *pfx)
{
#ifdef TIME
  printf("%s: %f\n", pfx, getTime());
#endif
}

// Returns the longest repeating non-overlapping
// substring in str
string longestRepeatedSubstring(string &str)
{
    int n = (int)str.length();
    // Setting all to 0
    memset(re2, 0, sizeof(LCSRe2));
    string res; // To store result
    int res_length = 0; // To store length of result
 
    // building table in bottom-up manner
    int i, index = 0;
    for (i=1; i<=n; i++)
    {
#ifdef DEBUG
printf("i%d: ", i);
#endif
       // memset(re1, 0, sizeof(LCSRe1[0]) * n);
        for (int j=i+1; j<=n; j++)
        {
            if ( str[i-1] == str[j-1] )
            {
                re1[j] = re2[j-1] + 1;
 
                // updating maximum length of the
                // substring and updating the finishing
                // index of the suffix
                if (re1[j] > res_length)
                {
                    res_length = re1[j];
                    index = max(i, index);
                }
            }
            else
                re1[j] = 0;
#ifdef DEBUG
 printf("%d ", re1[j]);
#endif
        }
#ifdef DEBUG
 printf("\n");
#endif
        swap(re1, re2);
    }
 
    // If we have non-empty result, then insert all
    // characters from first character to last
    // character of string
    if (res_length > 0)
        for (i = index - res_length + 1; i <= index; i++)
            res.push_back(str[i-1]);
 
    return res;
}
 
// Driver program to test the above function
int main()
{
  ios::sync_with_stdio(0); cin.tie(0);
  string s;
  cin >> s;
  auto res = longestRepeatedSubstring(s);
  if ( res.empty() ) puts("-1");
  else printf("%s\n", res.c_str());
printTime("res");
}
