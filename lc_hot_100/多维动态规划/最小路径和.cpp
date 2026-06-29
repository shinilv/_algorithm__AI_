/*
题意：
一个机器人位于一个 m x n 网格的左上角 （起始点在下图中标记为 “Start” ）。

机器人每次只能向下或者向右移动一步。机器人试图达到网格的右下角（在下图中标记为 “Finish” ）。

问总共有多少条不同的路径？
1 < n, m <= 100
思路：
先定义状态，dp[i][j] 表示 到第i行，第j列这个点的路径和
思考转移方程 可以从上面或者左边走过来，所以dp[i][j] = dp[i - 1][j] + dp[i][j - 1]
如何初始化。可以容易得到，i = 1 或 j = 1的情况下，只有一条路径
*/


class Solution {
public:
    int uniquePaths(int m, int n) {
        vector<vector<int>> dp(m + 1, vector<int>(n + 1));
        for (int i = 1; i <= n; i++) dp[1][i] = 1;
        for (int i = 1; i <= m; i++) dp[i][1] = 1;
        for (int i = 2; i <= m; i++) {
            for (int j = 2; j <= n; j++) dp[i][j] = dp[i - 1][j] + dp[i][j - 1];
        }
        return dp[m][n];
    }
};
