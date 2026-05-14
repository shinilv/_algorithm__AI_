/*
题意：
给定两个字符串 s 和 t，长度分别是 m 和 n，返回 s 中的 最短窗口 子串，
使得该子串包含 t 中的每一个字符（包括重复字符）。
如果没有这样的子串，返回空字符串 ""
思路：
首先使用一个cnt[26]数组记录t中每个字符的个数
然后考虑滑动窗口找满足条件的最短窗口
使用一个cur[26] 的数组去记录此时窗口内的每个字符的个数
额外需要一个变量sum = t.size()
滑动窗口的同时比较cur和cnt数组，更新sum
如果sum == 0，可以更新答案（如果此时窗口小于ans.size()
*/


class Solution {
public:
    string minWindow(string s, string t) {
        int n = s.size(), m = t.size();
        int cnt[200], cur[200];
        for (char ch : t) {
            cnt[ch]++;
        }
        int sum = m;
        int ansl = -1, ans = n + 1;
        for (int l = 0, r = 0; r < n; r++) {
            if (cur[s[r]]++ < cnt[s[r]]) {
                sum--;
            }
            while (sum == 0) {
                if (r - l + 1 < ans) {
                    ans = r - l + 1;
                    ansl = l;
                }
                if (cur[s[l]]-- == cnt[s[l]]) {
                    sum++;
                }
                l++;
            }
        }
        if (ans> n) return "";
        return s.substr(ansl, ans);
    }
};
