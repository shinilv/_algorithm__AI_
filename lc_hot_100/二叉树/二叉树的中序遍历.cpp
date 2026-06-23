/*
题意：
给定一个二叉树根节点，返回它的中序遍历
思路：
左 - 根 - 右
*/

class Solution {
public:
    vector<int> inorderTraversal(TreeNode* root) {
        vector<int> ans;
        auto dfs = [&](auto&& dfs, TreeNode* node) -> void {
            if (node == nullptr) {
                return;
            }
            dfs(dfs, node->left);
            ans.push_back(node->val);
            dfs(dfs, node->right);
        };
        dfs(dfs, root);
        return ans;
    }
};
