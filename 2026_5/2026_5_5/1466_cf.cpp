/*
来自灵茶の试炼
https://codeforces.com/problemset/problem/1466/C

输入 T(≤1e5) 表示 T 组数据。所有数据的字符串长度之和 ≤1e5。
每组数据输入长度 ≤1e5 的字符串 s，只包含小写英文字母。

修改 s 中的字母，使得 s 不含任何长度大于 1 的回文子串。
输出最少修改次数。

思路：
由于长度大于 3 的回文串，内部一定包含长度为 2 或 3 的回文子串，所以只要保证没有长度为 2 或 3 的回文子串。
这意味着不能出现 s[i] == s[i-1] 或者 s[i] == s[i-2] 的情况。
当出现这种情况时，改 s[i] 可以断开与更右边的字母的联系，相比改 s[i-1] 或者 s[i-2] 更好。
注意 s[i] 不能改成 s[i+1] 或者 s[i+2]，否则可能会形成新的回文子串。
*/

#include <bits/stdc++.h>
using namespace std;

void solve() {
    string s;
    cin >> s;
    int ans = 0;
    int n = s.size();
    for (int i = 1; i < n; i++) {
        if (s[i] == s[i - 1] || (i > 1 && s[i] == s[i - 2])) {
            ans++;
            for (char x = 'a'; x <= 'z'; x++) {
                if (x != s[i - 1] && (i <= 1 || x != s[i - 2]) && (i + 1 >= n || x != s[i + 1]) && (i + 2 >= n || x != s[i + 2])) {
                    s[i] = x;
                    break;
                }
            }
        }
    }
    cout << ans << '\n';

}

int main() {
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}