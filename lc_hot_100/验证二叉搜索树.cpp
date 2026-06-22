/*
题意：
给你一个二叉树的根节点 root ，判断其是否是一个有效的二叉搜索树。

有效 二叉搜索树定义如下：

节点的左子树只包含 严格小于 当前节点的数。
节点的右子树只包含 严格大于 当前节点的数。
所有左子树和右子树自身必须也是二叉搜索树。

       5
  4         6
         3     7

思路：
递归判断，对每个子树进行最大最小值的约束
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
class Solution {
public:
    bool dfs(TreeNode* node, long long mi, long long mx) {
        if (node == nullptr) return true;
        if (node->val <= mi || node->val >= mx) return false;
        return dfs(node->left, mi, node->val) && dfs(node->right, node->val, mx);
    }
    bool isValidBST(TreeNode* root) {
        return dfs(root, LLONG_MIN / 2, LLONG_MAX / 2);
    }
};
