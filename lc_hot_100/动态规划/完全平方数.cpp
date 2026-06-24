/*
题意： 给你一个整数 n，返回和为 n 的完全平方数的最少数量。
完全平方数是一个整数，其值等于另一个整数的平方。
思路：
完全背包
*/

class Solution {
public:
    int numSquares(int n) {
        vector<int> temp;
        for (int i = 1; i * i <= n; i++) {
            temp.push_back(i * i);
        }
        if (temp.back() == n) return 1;
        vector<int> dp(n + 1, n + 1);
        dp[0] = 0;
        for (int i = 0; i < temp.size(); i++) {
            for (int j = temp[i]; j <= n; j++) {
                dp[j] = min(dp[j], dp[j - temp[i]] + 1);
            }
        }
        return dp[n];
    }
};
