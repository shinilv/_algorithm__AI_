/*
题意：
给你一个链表的头节点，反转这个链表，返回反转之后的头节点
思路：
1：循环：使用两个指针分别为pre = nullptr, cur = head
遍历这个链表，记录当前节点指向的下一个nxt = cur->next, 令cur->next = pre, pre = cur, cur = nxt

2. 递归
先假设当前节点之后的节点已经全部完成了反转，并且返回给你新的头节点，
如果当前节点是最后一个节点或者空节点直接返回，否则让当前节点的下一个的下一个节点指向当前节点
当前节点的下一个节点置为空，递归下去
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
    ListNode* reverseList(ListNode* head) {
        ListNode *cur = head, *pre = nullptr;
        while (cur) {
            ListNode* nxt = cur->next;
            cur->next = pre;
            pre = cur;
            cur = nxt;
        }
        return pre;
    }
};
// 1 2 3 4 5
// 1 2 3 5 4   // 1 -> 2 -> 3 -> 4 <- 5(此时执行head->next->next = head;head->next = nullptr;)   --->   1 -> 2 -> 3 <- 4
// <- 5 返回的newhead 依然是5
class Solution {
   public:
    ListNode* reverseList(ListNode* head) {
        if (!head || !head->next) {
            return head;
        }
        ListNode* newhead = reverseList(head->next);
        head->next->next = head;
        head->next = nullptr;
        return newhead;
    }
};
