"""
题意：给一个二进制字符串 s，你可以把任意位置的 0/1 翻转。
目标是让最终字符串里不存在子序列 "011" 和 "110"。求最少翻转次数。

思路：
这里分情况看，全0 全1 的情况计算需要修改几次
'1' 的数量为1的情况，修改次数为0
'1' 的数量为2的情况，只能是首尾 （如果首尾出现0，并且1的数量为2，那一定含有题目的两种子序列）
'1' 的数量大于2的情况是一定不满足条件的
"""

class Solution:
    def minFlips(self, s: str) -> int:
        n = len(s)
        ans = min(s.count('1'), s.count('0'))
        if s.count('1') >= 1:
            ans = min(ans, s.count('1') - 1)
        if s[0] == '1' and s[-1] == '1' and n >= 2:
            ans = min(ans, s.count('1') - 2)
        return ans