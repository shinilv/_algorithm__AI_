"""
/*
题意：
https://atcoder.jp/contests/abc400/tasks/abc400_c

输入 n(1≤n≤1e18)。
如果正整数 x 可以表示为 pow(2, a) * b * b，其中 a 和 b 均为正整数，则称 x 为好整数。
输出 [1, n] 中的好整数个数。

思路：
x 本质只有两类：2 * b^2 或者 4 * b^2，这两类无交集。
对于 8 * b^2，等价于 2 * (2b)^2，其余类似。

"""

n = int(input())

def func(v:int) -> int:
    l, r = 0, n // v
    while l < r:
        m = l + r + 1 >> 1
        if m * m * v <= n:
            l = m
        else:
            r = m - 1
    return l

print(func(2) + func(4))

