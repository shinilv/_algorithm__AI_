/*
https://codeforces.com/problemset/problem/611/C

输入 n m(1≤n,m≤500) 和 n 行 m 列的网格图，只包含 '.' 和 '#'，分别表示空地和障碍物。
然后输入 q(1≤q≤1e5) 和 q 个询问，每个询问输入 r1 c1 r2 c2，表示子矩阵的左上角和右下角。下标从 1 开始。

对于每个询问，计算子矩阵有多少种（在空地上）放置恰好一个多米诺骨牌的方案数（把两个相邻空地涂黑的方案数）。
*/

#include <bits/stdc++.h>
using namespace std;

int main() {
    int t = 1;
    while (t--) {
        int n, m;
        cin >> n >> m;
        vector<string> grid(n);
        for (int i = 0; i < n; i++) {
            cin >> grid[i];
        }
        vector<vector<int>> pre(n + 1, vector<int>(m + 1, 0)), 
            pre_row(n + 1, vector<int>(m + 1, 0)), pre_col(n + 1, vector<int>(m + 1, 0));
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                pre_col[i][j] = pre_col[i - 1][j] + (grid[i - 1][j - 1] == '.' && j < m && grid[i - 1][j] == '.');
                pre_row[i][j] = pre_row[i][j - 1] + (grid[i - 1][j - 1] == '.' && i < n && grid[i][j - 1] == '.');
                pre[i][j] = pre[i - 1][j] + pre[i][j - 1] - pre[i - 1][j - 1];
                if (grid[i - 1][j - 1] == '.') {
                    if (i < n && grid[i][j - 1] == '.') {
                        pre[i][j]++;
                    }
                    if (j < m && grid[i - 1][j] == '.') {
                        pre[i][j]++;
                    }
                }
            }
        }
        int q;
        cin >> q;
        while (q--) {
            int r1, c1, r2, c2;
            cin >> r1 >> c1 >> r2 >> c2;
            int ans = pre[r2][c2] - pre[r1 - 1][c2] - pre[r2][c1 - 1] + pre[r1 - 1][c1 - 1];
            // 减去最后一行和最后一列
            ans -= (pre_row[r2][c2] - pre_row[r2][c1 - 1]);
            ans -= (pre_col[r2][c2] - pre_col[r1 - 1][c2]);

            cout << ans << endl;
        }
    }
}