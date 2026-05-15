/*
题意： 给定一个字符串 s ，请你找出其中不含有重复字符的 最长 子串 的长度。
思路：
考虑滑动窗口，使用一个cnt变量，计算窗口内字符出现次数 > 1 的数量，
如果cnt == 0 可以更新答案
使用一个cur数组记录窗口内每种字符出现的次数，便于维护窗口的cnt
*/

class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        int n  = s.size(), ans = 0, cnt = 0;
        unordered_map<int, int> cur;
        for (int l = 0, r = 0; r < n; r++) {
            if (cur[s[r] - 'a']++ == 1) cnt++;
            while (cnt) {
                if (--cur[s[l++] - 'a'] == 1) {
                    cnt--;
                }
            }
            ans = max(ans, r - l + 1);
        }
        return ans;
    }
};
