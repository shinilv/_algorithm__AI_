/*
题意：
给你两个单词 word1 和 word2， 请返回将 word1 转换成 word2 所使用的最少操作数  。
你可以对一个单词进行如下三种操作：
插入一个字符
删除一个字符
替换一个字符
n, m = len(word1), len(word2)
要求时间复杂度 O(n * m)

思路：
定义状态dp[i][j] 表示 word1[0 - i] 和 word2[0 - j] 匹配需要的最小操作次数
思考状态转移方程（根据执行的操作）
if (word1[i] == word2[j]) dp[i][j] = dp[i - 1][j - 1]; // 不执行操作
执行操作一： dp[i][j] = min(dp[i - 1][j], dp[i][j - 1]) + 1
执行操作二： dp[i][j] = min(dp[i - 1][j], dp[i][j - 1]) + 1;
执行操作三： dp[i][j] = dp[i - 1][j - 1] + 1;

综上
if (word1[i] == word2[j]) dp[i][j] = dp[i - 1][j - 1];
dp[i][j] = min(dp[i][j], min(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]) + 1);

初始化：
dp[0][j] = dp[i][0] = i / j;  0 <= i < n, 0 <= j < m
其他dp[i][j] = n + m

sea
eat
*/

class Solution {
public:
    int minDistance(string word1, string word2) {
        int n = word1.size(), m = word2.size();
        vector<vector<int>> dp(n + 1, vector<int>(m + 1, n + m));
        for (int i = 0; i < n; i++) dp[i][0] = i;
        for (int i = 0; i < m; i++) dp[0][i] = i;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (word1[i] == word2[j]) {
                    dp[i + 1][j + 1] = dp[i][j];
                }
                dp[i + 1][j + 1] = min(dp[i + 1][j + 1], min({dp[i][j + 1], dp[i + 1][j], dp[i][j]}) + 1);
            }
        }
        return dp[n][m];
    }
};
