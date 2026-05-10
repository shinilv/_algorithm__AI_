"""
题意：给你一个数组 nums 和整数 target，从下标 0 出发，
每次可以向右跳到下标 j，要求 nums[j] - nums[i] 的差值在 [-target, target] 之间。
问最多跳多少次能到达最后一个下标 n - 1；如果到不了，返回 -1。

思路：dfs(i) 表示下标n到 i 最大跳跃次数 
"""

class Solution:
    def maximumJumps(self, nums: List[int], target: int) -> int:
        @cache
        def dfs(j: int) -> int:
            if j == 0:
                return 0
            res = -inf
            for i in range(j):
                if abs(nums[i] - nums[j]) <= target:
                    res = max(res, dfs(i) + 1)
            return res
        ans = dfs(len(nums) - 1)
        return -1 if ans < 0 else ans 
                