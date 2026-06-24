/*
题意： 给你一个整数数组 coins，表示不同面额的硬币；以及一个整数 amount，表示总金额。
请你计算并返回可以凑成总金额所需的最少硬币个数。如果没有任何一种硬币组合能组成总金额，返回 -1。
你可以认为每种硬币的数量是无限的。

思路：
*/

class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        int n = coins.size();
        vector<vector<int>> dp(n + 1, vector<int>(amount + 1, amount + 1));
        for (int i = 0; i <= n; i++) dp[i][0] = 0;
        // for (int i = 0; i < n; i++) {
        //     for (int j = 0; j <= amount; j++) {
        //         dp[i + 1][j] = dp[i][j]; // 不选第 i 个硬币
        //         for (int k = 1; k * coins[i] <= j; k++) {
        //             dp[i + 1][j] = min(dp[i + 1][j], dp[i][j - k * coins[i]] + k);
        //         }
        //     }
        // }

/*
这是最暴力的写法，我们可以发现
暴力枚举 dp[i + 1][j] = min(dp[i][j], dp[i][j - c] + 1, dp[i][j - 2c] + 2, ...)
dp[i + 1][j - c]  中已经包含了 dp[i][j - c], dp[i][j - 2c] + 1, dp[i][j - 3c] + 2 ...
*/
        for (int i = 0; i < n; i++) {
            int c = coins[i];
            for (int j = 0; j <= amount; j++) {
                dp[i + 1][j] = dp[i][j];
                if (j >= c) {
                    dp[i + 1][j] = min(dp[i + 1][j], dp[i + 1][j - c] + 1);
                }
            }
        }

        if (dp[n][amount] > amount) return -1;
        return dp[n][amount];
    }
};


// 滚动数组优化
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        int n = coins.size();
        vector<int> dp(amount + 1, amount + 1);
        dp[0] = 0;
        for (int i = 0; i < n; i++) {
            for (int j = coins[i]; j <= amount; j++) {
                dp[j] = min(dp[j], dp[j - coins[i]] + 1);
            }
        }
        if (dp[amount] > amount) return -1;
        return dp[amount];
    }
};
