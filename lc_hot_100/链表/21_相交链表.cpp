/*
题意：
给定两个链表的头节点， 找到它们相交的起始节点
要求设计O(n + m) 的时间复杂度， O(1) 空间复杂度

思路：
如果O(n) 时间复杂度的情况，可以使用unordered_set记录一条链表的所有结点，再遍历另一条节点

O(1) 空间复杂度
第一个链表A的长度 n， 第二个链表B的长度为m
令PA 执行链表A的头节点，PB执行链表B的头节点
同时让PA, PB向后移动
如果n == m，那么期间一定会相遇，返回相遇的节点（不相交也会同时为nullptr）
如果n != m， 那么PA 走到最后之后执行headB, PB 走到最后之后执行headA
这样继续走，第二遍一定会相遇，即使没有相交也会同时走到最后变为空节点

以上这种做法相当于让长链表先走abs(n - m) 步，然后短链表在开始走
*/

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
   public:
    ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
        if (headA == nullptr || headB == nullptr) {
            return nullptr;
        }
        ListNode *PA = headA, *PB = headB;
        while (PA != PB) {
            PA = PA == nullptr ? headB : PA->next;
            PB = PB == nullptr ? headA : PB->next;
        }
        return PA;
    }
};
