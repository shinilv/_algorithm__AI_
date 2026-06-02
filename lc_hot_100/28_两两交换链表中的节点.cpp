/*
题意：给定一个链表，两两交换其中相邻的节点，并返回交换后的链表头节点。
思路：使用递归或迭代的方法，每次处理两个节点，交换它们的位置，然后递归或迭代处理剩余的节点。
具体实现参考代码。
*/


/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    ListNode* swapPairs(ListNode* head) {
        if (head == nullptr || head->next == nullptr) {
            return head;
        }
        ListNode* ret = head->next;
        head->next = swapPairs(head->next->next);
        ret->next = head;
        return ret;
    }
};
