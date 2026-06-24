/*
题意： 假设你正在爬楼梯，需要 n 阶你才能到达楼顶。
每次你可以爬 1 或 2 个台阶，请问有多少种不同的方法可以爬到楼顶？
数据范围：
1 <= n <= 45

思路：
经典斐波那契数列
*/

class Solution {
public:
    int climbStairs(int n) {
        vector<int> f(n + 1);
        f[0] = f[1] = 1;
        for (int i = 2; i <= n; i++) f[i] = f[i - 1] + f[i - 2];
        return f[n];
    }
};
