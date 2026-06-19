"""
https://atcoder.jp/contests/abc459/tasks/abc459_d

输入 T(≤3e5) 表示 T 组数据。所有数据的字符串长度之和 ≤1e6。
每组数据输入长度 ≤1e6 的字符串 s，只包含小写英文字母。

重新排列 s，使得 s 中的相邻字母均不同。
如果无法做到，输出 No；否则输出 Yes 和重排后的 s。多解输出任意解。

输入
3
aiiw
doodoo
aabbababcacababaaba
输出
Yes
iwai
No
Yes
ababacabababacababa
"""
from collections import defaultdict
import heapq

for _ in range(int(input())):
    s = input()
    cnt = defaultdict(int)
    for c in s:
        cnt[c] += 1
    ans = []
    mx = 0
    for k, v in cnt.items():
        heapq.heappush(ans, (-v, k))
        mx = max(mx, v)
    if mx > len(s) - mx + 1:
        print("No")
        continue
    ot = []
    i, j = 0, len(ans) - 1
    while len(ans) > 1:
        v1, c1 = heapq.heappop(ans)
        v2, c2 = heapq.heappop(ans)
        ot.append(c1)
        ot.append(c2)
        v1 += 1
        v2 += 1
        if v1 < 0:
            heapq.heappush(ans, (v1, c1))
        if v2 < 0:
            heapq.heappush(ans, (v2, c2))
    if len(ot) < len(s):
        ot.append(ans[0][1])
    print("Yes")
    print("".join(map(str, ot)))


