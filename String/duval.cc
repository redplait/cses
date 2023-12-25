#include <iostream>
#include <vector>

using namespace std;

// solution for Minimal Rotation
// ripped Duval algo from https://codeforces.com/blog/entry/90035

/// Find starting position of minimum acyclic string in (s)
void min_cyc(string &s)
{
    int n = (int)s.size(); /// the real size of the string
    s += s; /// for convention since we are deadling with acyclic

    ///
    /// s = s1 + s2 + s3
    /// s1 = s[1..l-1] is handled
    /// s2 = s[l..r]   is handling
    /// s3 = s[p..n]   is going to be handled
    /// 

    int res = 0; /// minimum acyclic string
    /// while (s2) is a lyndon word, try to add s2 with s[p]
    for (int l = 0; l < n; )
    {
        ///
        /// - Case 1: 
        ///     If (s) is fully ordered, then return 0
        ///     Surely will this loop make [l..r] = [0..n-1]
        ///     Ans it is currently that (l = 0) 
        ///     => res = l is a correct answer
        ///
        /// - Case 2:
        ///     Minimum acyclic string s' = s[l..r] that 0 <= l < n <= r < 2n
        ///     Also if s2 is s', then the loop will extend its (r >= n)
        ///     Since l < n, the latest (l) will create s'    
        ///     => res = l is a correct answer
        /// 
        /// Hence in both cases, res = last(l) will return a correct answer
        ///
        res = l;

        /// Extend as much as possible lyndon word s2 = s[l..r]
        int r = l, p = l + 1;
        while (p < (int)s.size())
        {
            /// (s2 + s[p]) is not a lyndon word
            if (s[r] > s[p]) 
            {
                break;
            }

            /// (s2 + s[p]) is stil a lyndon word, hence extend s2
            if (s[r] == s[p]) 
            {
                ++r;
                ++p;
                continue;
            }
            
            /// (s2 + s[p]) is a lyndon word, but it may be a repeated string
            if (s[r] < s[p]) 
            {
                r = l;
                ++p;
                continue;
            }
        }

        /// The lyndon word may have the form of s2 = sx + sx + .. + sx like "12312123"
        while (l <= r) 
        {
            /// s[l..l + p - r] is sx
            l += p - r;
        }
    }
    printf("%s\n", s.substr(res, n).c_str());
}

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
    string s;
    cin >> s;
    min_cyc(s);
}
