"""
https://codeforces.com/problemset/problem/2175/B

输入 T(≤1e4) 表示 T 组数据。所有数据的 n 之和 ≤5e5。
每组数据输入 n(2≤n≤4e5) L R(1≤L<R≤n)。
构造一个长为 n 的数组 a(1≤a[i]≤1e9)，
满足只有子数组 [L,R] 的异或和等于 0，其余非空子数组的异或和均不为 0。

思路：构造一个前缀异或和数组 s，s[i] 表示前 i 个数的异或和，那么 a[i] = s[i-1] ^ s[i]。
我们可以让 s[i] = i + 1，除了 s[r] = l - 1，这样就满足了题目的要求。
"""

for _ in range(int(input())):
    n, l, r = map(int, input().split())
    s = [0] * n
    a = [0] * n
    s[0] = 1
    a[0] = 1
    for i in range(1, n):
        s[i] = i + 1 if i + 1 != r else l - 1
        a[i] = s[i - 1] ^ s[i]
    print(*a)