/*
题意：给你一个字符串数组，请你将字母异位词（排序后相同）组合在一起。可以按任意顺序返回结果列表。
思路：unordered_map<string, vector<string>> mp 的使用
*/

class Solution {
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        unordered_map<string, vector<string>> mp;
        for (auto& x : strs) {
            string s = x;
            // 排序
            ranges::sort(s);
            mp[s].push_back(x);
        }
        vector<vector<string>> ans;
        for (auto& [k, v] : mp) {
            ans.push_back(v);
        }
        return ans;
    }
};