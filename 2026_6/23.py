"""
题意：
https://atcoder.jp/contests/abc461/tasks/abc461_d
输入 n m(1≤n,m≤500) k(0≤k≤n*m) 和 n 行 m 列的 01 矩阵 a。
输出 a 有多少个非空子矩阵，元素和恰好为 k。

思路：
滑动窗口 + 前缀和
（超时了，不会优化
"""
from collections import defaultdict

n, m, k = map(int, input().split())
a = [input().strip() for _ in range(n)]
if n < m:
    b = []
    for j in range(m):
        row = []
        for i in range(n):
            row.append(a[i][j])
        b.append(''.join(row))
    a = b
    n, m = m, n
pre = [[0] * (m + 1) for _ in range(n)]
for i in range(n):
    for j in range(0, m):
        pre[i][j + 1] = pre[i][j] + (1 if a[i][j] == '1' else 0)
ans = 0
for i in range(m):
    for j in range(i, m):
        r, sum = 0, 0
        cnt = defaultdict(int)
        cnt[0] = 1
        while r < n:
            sum += pre[r][j + 1] - pre[r][i]
            ans += cnt[sum - k]
            cnt[sum] += 1
            r += 1
print(ans)
