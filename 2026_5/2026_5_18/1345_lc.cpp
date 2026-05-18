/*
题意：
给你一个整数数组arr， 初始在下标0处
每一步可以 i + 1, i - 1, j（满足arr[j\ == arr[i] && i != j）
返回到达最后一个下标所需要的最小操作数

思路：
使用哈希桶记录一下每个元素对应的所有下标，
然后bfs搜索，第一次搜到最后一个下标在第几层，就返回多少即可
使用一个标记数组，去重
还需要使用一个哈希表，记录已经遇到过的数，防止大量重复循环
*/

class Solution {
   public:
    int minJumps(vector<int>& arr) {
        int n = arr.size();
        unordered_map<int, vector<int>> mp;
        unordered_map<int, int> cnt;  // 优化时间复杂度，去重，如果一个数已经出现过也去掉
        for (int i = 0; i < n; i++) {
            mp[arr[i]].push_back(i);
        }
        vector<int> vis(n);
        vector<int> q;
        vis[0] = 1;
        q.push_back(0);
        for (int ans = 0; q.size(); ans++) {
            vector<int> temp;
            for (auto x : q) {
                if (x == n - 1) {
                    return ans;
                }
                if (!cnt.count(arr[x])) {
                    for (auto v : mp[arr[x]]) {
                        if (vis[v]) continue;
                        temp.push_back(v);
                        vis[v] = 1;
                    }
                    cnt[arr[x]] = 1;
                }
                int l = x - 1, r = x + 1;
                if (l >= 0 && vis[l] == 0) {
                    temp.push_back(l);
                    vis[l] = 1;
                }
                if (r < n && vis[r] == 0) {
                    temp.push_back(r);
                    vis[r] = 1;
                }
            }
            q = move(temp);
        }
        return 0;
    }
};
