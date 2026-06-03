/*
题意：
https://atcoder.jp/contests/abc456/tasks/abc456_d
输入长度 ≤3e5 的字符串 s，只包含 'a' 'b' 'c'。
输出 s 中的相邻字母均不相同的非空子序列个数，模 998244353。
完全一样但位置不同的子序列，算作不同的子序列。



思路：
考虑dp，dp[i][j] 表示到 i 这个位置，最后一个字符是 j
的相邻字母均不相同的非空子序列个数。
j = 'a' 'b' 'c' 分别对应 0 1 2。
对于 dp[i][j] dp[i][j] = dp[i - 1][j] + 1
否则 dp[i][j] = dp[i - 1][j] + dp[i - 1][k]，其中 k != j。
dp[i][k] = dp[i - 1][k] 对于 k != j。
最后答案是 dp[n - 1][0] + dp[n - 1][1] + dp[n - 1][2]。
*/

#include <bits/stdc++.h>
using namespace std;

const int mod = 998244353;
int main() {
    string s;
    cin >> s;
    int n = s.size();
    vector<array<long long, 3>> dp(n, {0, 0, 0});
    dp[0][s[0] - 'a'] = 1;
    for (int i = 1; i < n; i++) {
        int j = s[i] - 'a';
        dp[i][j] = (1 + dp[i - 1][j]) % mod;
        for (int k = 0; k < 3; k++) {
            if (k != j) {
                dp[i][j] = (dp[i][j] + dp[i - 1][k]) % mod;
                dp[i][k] = dp[i - 1][k];
            }
        }
    }
    long long ans = 0;
    for (int j = 0; j < 3; j++) {
        ans = (ans + dp[n - 1][j]) % mod;
    }
    cout << ans << '\n';
    return 0;
}
