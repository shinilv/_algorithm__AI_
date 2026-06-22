/*
题意：
给你一棵二叉树的根节点，返回该树的 直径 。
二叉树的 直径 是指树中任意两个节点之间最长路径的 长度 。这条路径可能经过也可能不经过根节点 root 。
两节点之间路径的 长度 由它们之间边数表示。

思路：
对于某一个节点来说，经过它的最长路径 = 左子树高度 + 右子树高度
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
    int ans = 0;
    int max_h(TreeNode* node) {
        if (node == nullptr) {
            return 0;
        }
        int l = max_h(node->left);
        int r = max_h(node->right);
        ans = max(ans, l + r);
        return max(l, r) + 1;
    }
    int diameterOfBinaryTree(TreeNode* root) {
        max_h(root);
        return ans;
    }
};
