/*
题意： 给你一个只包含 '(' 和 ')' 的字符串，找出最长有效括号子串的长度。

数据范围：
0 <= s.length <= 3 * 10^4
s[i] 为 '(' 或 ')'

思路：
本题有两种做法
dp做法，这个比较难想的点就在于转移方程
dp[i] 表示以i结尾的有效括号字串的长度最大值
我们先考虑如果s[i] == '(' 那么dp[i] 一定等于0
s[i] == ')'的时候
先看情况1 如果s[i - 1] == '(' 那么dp[i] 一定 = dp[i - 2] + 2
(((())())
否则 看这个例子的最后一个元素 "(((())())" 我们可以得到dp[i - 1] 的值，
然后去看s[i - dp[i - 1] - 1] 是否是'('， 如果是，那么dp[i] = dp[i - dp[i - 1] - 1] + 2;
转移方程就这两种，本题边界细节较多，具体可参考代码
*/

class Solution {
public:
    int longestValidParentheses(string s) {
        int n = s.size();
        vector<int> dp(n);
        int ans = 0;
        for (int i = 1; i < n; i++) {
            if (s[i] == '(') continue;
            if (s[i - 1] == '(') {
                dp[i] = (i >= 2 ? dp[i - 2] : 0) + 2;
            } else if (i - dp[i - 1] - 1 >= 0 && s[i - dp[i - 1] - 1] == '('){
                dp[i] = (i - dp[i - 1] - 2 >= 0 ? dp[i - dp[i - 1] - 2] : 0) + 2 + dp[i - 1];
            }
            ans = max(ans, dp[i]);
        }
        return ans;
    }
};
