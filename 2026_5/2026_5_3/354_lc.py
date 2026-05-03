"""
题意：给一堆信封，每个信封有宽和高，只有宽高都严格更大的信封才能套住另一个，求最多能套多少层。

思路：先按宽度升序排序，宽度相同按高度降序排序，然后问题转化为在高度数组上求最长严格递增子序列 LIS
"""

class Solution:
    def maxEnvelopes(self, envelopes: List[List[int]]) -> int:
        envelopes.sort(key = lambda x: (x[1], -x[0]))
        g = []
        for l, _ in envelopes:
            j = bisect_left(g, l)
            if j < len(g):
                g[j] = l
            else:
                g.append(l)
        return len(g)