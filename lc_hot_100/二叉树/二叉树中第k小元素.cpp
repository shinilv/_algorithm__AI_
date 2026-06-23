/*
题意：
给定一个二叉搜索树的根节点 root ，和一个整数 k ，
请你设计一个算法查找其中第 k 小的元素（k 从 1 开始计数）。
思路：
核心代码：
    ans, cnt = 0, 0
        if (cnt > k) return ans
        kthSmallest(root->left, k);
        cnt++;
        if (cnt == k) ans = root->val;
        kthSmallest(root->right, k);
        return ans;
*/

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
  1
     2
class Solution {
public:
    unordered_map<TreeNode*, int> mp;
    int kthSmallest(TreeNode* root, int k) {
        auto dfs = [&](auto&& dfs, TreeNode* node) -> int {
            if (node == nullptr) {
                return 0;
            }
            mp[node] = dfs(dfs, node->left);
            return mp[node] + dfs(dfs, node->right) + 1;
        };
        dfs(dfs, root);
        auto dfs2 = [&](auto&& dfs2, TreeNode* node, int x) -> int {
            int l = mp[node];
            if (x > l + 1) {
                return dfs2(dfs2, node->right, x - l - 1);
            } else if (x < l + 1) {
                return dfs2(dfs2, node->left, x);
            } else {
                return node->val;
            }
        };

        return dfs2(dfs2, root, k);
    }
};



class Solution {
public:
    int ans = 0, cnt = 0;
    int kthSmallest(TreeNode* root, int k) {
        if (root == nullptr) return 0;
        if (cnt > k) {
            return ans;
        }
        kthSmallest(root->left, k);
        cnt++;
        if (cnt == k) ans = root->val;
        kthSmallest(root->right, k);
        return ans;
    }
};
