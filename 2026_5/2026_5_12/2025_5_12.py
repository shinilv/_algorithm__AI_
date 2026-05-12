"""
题意：给你一组任务，每个任务有实际消耗能量 actual 和开始前最低需要能量 minimum。

你可以任意安排任务顺序，要求求出一个最小的初始能量值，使得按某种顺序能完成所有任务。

思路：先说结论， 要想启动能量最小，按照actual - mininum 降序排序即可
大概观察得到的猜想：要想启动能量最少，就要最后剩余能量最少，
就要把能量差小的任务放在后面完成，比如一个(19,20)的任务一个(10,20) 的任务，
把第一个任务放在后面完成就只剩余1，把第二个放后面就会剩余10，
为了不浪费启动能量就要按第一种完成任务
严谨证明：这是数学题，感兴趣的点击这里https://leetcode.cn/problems/minimum-initial-energy-to-finish-tasks/solutions/3965592/jiao-huan-lun-zheng-fa-pythonjavaccgojsr-ozft/?envType=daily-question&envId=2026-05-12
"""


class Solution:
    def minimumEffort(self, tasks: List[List[int]]) -> int:
        tasks.sort(key=lambda t : t[0] - t[1])

        ans = 0
        s = 0
        for a, m in tasks:
            ans = max(ans, s + m)
            s += a
        return ans