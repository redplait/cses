#include <stdio.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <numeric>
#include <unordered_map>
#include <vector>

using namespace std;

// bad solution for Houses and Schools
// based on Lloyg algo ripped from https://github.com/dstein64/kmeans1d/blob/master/kmeans1d/_core.cpp

typedef unsigned long ulong;

/*
 *  Internal implementation of the SMAWK algorithm.
 */
template <typename T>
void _smawk(
        const vector<ulong>& rows,
        const vector<ulong>& cols,
        const function<T(ulong, ulong)>& lookup,
        vector<ulong>* result) {
    // Recursion base case
    if (rows.size() == 0) return;

    // ********************************
    // * REDUCE
    // ********************************

    vector<ulong> _cols;  // Stack of surviving columns
    for (ulong col : cols) {
        while (true) {
            if (_cols.size() == 0) break;
            ulong row = rows[_cols.size() - 1];
            if (lookup(row, col) >= lookup(row, _cols.back()))
                break;
            _cols.pop_back();
        }
        if (_cols.size() < rows.size())
            _cols.push_back(col);
    }

    // Call recursively on odd-indexed rows
    vector<ulong> odd_rows;
    for (ulong i = 1; i < rows.size(); i += 2) {
        odd_rows.push_back(rows[i]);
    }
    _smawk(odd_rows, _cols, lookup, result);

    unordered_map<ulong, ulong> col_idx_lookup;
    for (ulong idx = 0; idx < _cols.size(); ++idx) {
        col_idx_lookup[_cols[idx]] = idx;
    }

    // ********************************
    // * INTERPOLATE
    // ********************************

    // Fill-in even-indexed rows
    ulong start = 0;
    for (ulong r = 0; r < rows.size(); r += 2) {
        ulong row = rows[r];
        ulong stop = _cols.size() - 1;
        if (r < rows.size() - 1)
            stop = col_idx_lookup[(*result)[rows[r + 1]]];
        ulong argmin = _cols[start];
        T min = lookup(row, argmin);
        for (ulong c = start + 1; c <= stop; ++c) {
            T value = lookup(row, _cols[c]);
            if (c == start || value < min) {
                argmin = _cols[c];
                min = value;
            }
        }
        (*result)[row] = argmin;
        start = stop;
    }
}

/*
 *  Interface for the SMAWK algorithm, for finding the minimum value in each row
 *  of an implicitly-defined totally monotone matrix.
 */
template <typename T>
vector<ulong> smawk(
        const ulong num_rows,
        const ulong num_cols,
        const function<T(ulong, ulong)>& lookup) {
    vector<ulong> result;
    result.resize(num_rows);
    vector<ulong> rows(num_rows);
    iota(begin(rows), end(rows), 0);
    vector<ulong> cols(num_cols);
    iota(begin(cols), end(cols), 0);
    _smawk<T>(rows, cols, lookup, &result);
    return result;
}

/*
 *  Calculates cluster costs in O(1) using prefix sum arrays.
 */
template <typename T>
class CostCalculator {
  const vector<T>& m_vec;
  public:
    CostCalculator(const vector<T>& vec): m_vec(vec)
    { }

    T calc(int i, int j)
    {
      if ( i == j ) return T{};
      T res = abs(i - j) * (m_vec[j]);
#ifdef DEBUG
 printf("%d %d: %d\n", i, j, res);
#endif
      return res;
    }
};

template <typename T>
class Matrix {
    vector<T> data;
    ulong num_rows;
    ulong num_cols;

  public:
    Matrix(ulong num_rows, ulong num_cols) {
        this->num_rows = num_rows;
        this->num_cols = num_cols;
        data.resize(num_rows * num_cols);
    }

    inline T get(ulong i, ulong j) {
        return data[i * num_cols + j];
    }

    inline void set(ulong i, ulong j, T value) {
        data[i * num_cols + j] = value;
    }
};

template <typename T>
void cluster(
        vector<T> &array,
        ulong k,
        vector<ulong> &clusters,
        vector<ulong> &centroids)
{
    auto n = array.size();

    // ***************************************************
    // * Set D and T using dynamic programming algorithm
    // ***************************************************

    // Algorithm as presented in section 2.2 of (Gronlund et al., 2017).

    CostCalculator<T> cost_calculator(array);
    Matrix<T> D(k, n);
    Matrix<int> Tm(k, n);

    for (ulong i = 0; i < n; ++i) {
        D.set(0, i, cost_calculator.calc(0, i));
        Tm.set(0, i, 0);
    }

    for (ulong k_ = 1; k_ < k; ++k_) {
        auto C = [&D, &k_, &cost_calculator](int i, int j) -> T {
            int col = i < j - 1 ? i : j - 1;
            return D.get(k_ - 1, col) + cost_calculator.calc(j, i);
        };
        vector<ulong> row_argmins = smawk<T>(n, n, C);
        for (ulong i = 0; i < row_argmins.size(); ++i) {
            ulong argmin = row_argmins[i];
            auto min = C(i, argmin);
            D.set(k_, i, min);
            Tm.set(k_, i, argmin);
        }
    }

    // ***************************************************
    // * Extract cluster assignments by backtracking
    // ***************************************************

    // TODO: This step requires O(kn) memory usage due to saving the entire
    //       T matrix. However, it can be modified so that the memory usage is O(n).
    //       D and T would not need to be retained in full (D already doesn't need
    //       to be fully retained, although it currently is).
    //       Details are in section 3 of (Grønlund et al., 2017).

    vector<T> sorted_clusters(n);

    ulong t = n;
    ulong k_ = k - 1;
    ulong n_ = n - 1;
    // The do/while loop was used in place of:
    //   for (k_ = k - 1; k_ >= 0; --k_)
    // to avoid wraparound of an unsigned type.
    do {
        ulong t_ = t;
        t = Tm.get(k_, n_);
        double sum_w = 0.0, prod_wx = 0.0;
        // https://stackoverflow.com/questions/64714915/finding-the-center-of-1d-data
// printf("t for %d %d is %d\n", k_, n_, t);
        for (ulong i = t; i < t_; ++i) {
            sorted_clusters[i] = k_;
            prod_wx += array[i] * (i - t);
            sum_w += array[i];
        }
        centroids[k_] = round(prod_wx / sum_w) + t;
#ifdef DEBUG
 printf("c[%ld] is %ld %f / %f\n", k_, centroids[k_], prod_wx, sum_w);
#endif
        k_ -= 1;
        n_ = t - 1;
    } while (t > 0);

    // ***************************************************
    // * Order cluster assignments to match de-sorted
    // * ordering
    // ***************************************************

    for (ulong i = 0; i < n; ++i) {
        clusters.push_back(sorted_clusters[i]);
    }
}

int main()
{
  int n, k;
  cin>>n>>k;
  if ( n == k )
  {
    puts("0");
    return 0;
  }
  vector<ulong> c(n), cs, centroids(k);
  for ( int i = 0; i < n; i++ ) cin>>c[i];
  cluster(c, k, cs, centroids);
#ifdef DEBUG
  for ( int i = 0; i < k; i++ ) printf("%d ", centroids[i]);
  printf("\n cs: ");
  for ( int i = 0; i < n; i++ ) printf("%d ", cs[i]);
  printf("\n");
#endif
  ulong res = 0;
  CostCalculator calc(c);
  for ( int i = 0; i < n; i++ )
  {
    auto tmp = calc.calc( centroids[cs[i]], i );
#ifdef DEBUG
 printf("%d: cs %ld res %ld\n", i, centroids[cs[i]], tmp);
#endif
    res += tmp;
  }
  printf("%ld\n", res);
}