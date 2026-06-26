/*
题意： 给你一个字符串 s 和一个字符串列表 wordDict，判断 s 是否可以由 wordDict 中的一个或多个单词拼接而成。
字典中的单词可以重复使用。

数据范围：
1 <= s.length <= 300
1 <= wordDict.length <= 1000
1 <= wordDict[i].length <= 20
s 和 wordDict[i] 仅由小写英文字母组成
wordDict 中的所有字符串互不相同

思路：
令dp[i] 表示 s[0 - i] 是否可以被拼接

*/

class Solution {
public:
    bool wordBreak(string s, vector<string>& wordDict) {
        int n = s.size(), mx = 20;
        unordered_set<string> se;
        for (auto x : wordDict) {
            mx = max(int(x.size()), mx);
            se.insert(x);
        }
        vector<bool> dp(n + 1, false);
        dp[0] = true;
        for (int i = 0; i < n; i++) {
            for (int j = i; j >= 0 && j >= i - mx; j--) {
                if (se.count(s.substr(j, i - j + 1)) && dp[j]) {
                    dp[i + 1] = true;
                    break;
                }
            }
        }
        return dp[n];
    }
};
