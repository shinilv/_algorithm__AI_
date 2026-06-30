"""
题意：
给你一个字符串 s ，它只包含三种字符 a, b 和 c 。
请你返回 a，b 和 c 都 至少 出现过一次的子字符串数目。
思路：
滑动窗口，对于每个r 如果存在一个l 使得 s[l - r] 满足条件，那么 s[0 - r] 一定也满足条件
"""



class Solution:
    def numberOfSubstrings(self, s: str) -> int:
        cnt = defaultdict(int)
        ans = left = 0
        for c in s:
            cnt[c] += 1
            while len(cnt) == 3:
                out = s[left]  # 离开窗口的字母
                cnt[out] -= 1
                if cnt[out] == 0:
                    del cnt[out]
                left += 1
            ans += left
        return ans
