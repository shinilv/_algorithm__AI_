/*
题意：
https://leetcode.cn/problems/maximum-ice-cream-bars/description/?envType=daily-question&envId=2026-06-21

思路：
从小到大依次选择
两种做法
*/

O(nlongn)
class Solution {
public:
    int maxIceCream(vector<int>& costs, int coins) {
        sort(costs.begin(), costs.end());
        int sum = 0, n = costs.size();
        for (int i = 0; i < n; i++) {
            sum += coins[i];
            if (sum > coins) return i;
        }
        return n;
    }
};

O(n + max(costs))
class Solution {
public:
    int maxIceCream(vector<int>& costs, int coins) {
        int mx = ranges::max(costs);
        vector<int> cnt(mx + 1);
        for (int cost : costs) {
            cnt[cost]++;
        }

        // 按照价格从低到高买
        int ans = 0;
        for (int cost = 1; cost <= mx && cost <= coins; cost++) {
            int num = min(cnt[cost], coins / cost);
            coins -= cost * num; // 买 num 根雪糕
            ans += num;
        }
        return ans;
    }
};
