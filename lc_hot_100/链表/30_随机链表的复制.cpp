/*
题意：
复制一条链表，不仅要复制 next 关系，还要复制 random 关系，并且不能和原链表共用节点。
思路：
对于每一个节点，在它后面插入一个复制，这个节点的random先不要初始化
然后遍历链表，cur->next->random = cur->random->next
最后把链表拆分成两条链表，原链表和复制的链
*/


/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* next;
    Node* random;

    Node(int _val) {
        val = _val;
        next = NULL;
        random = NULL;
    }
};
*/
class Solution {
public:
    Node* copyRandomList(Node* head) {
        if (head == nullptr) return nullptr;
        Node* cur = head;
        while (cur) {
            Node* nxt = cur->next;
            cur->next = new Node(cur->val);
            cur->next->next = nxt;
            cur = nxt;
        }
        cur = head;
        while (cur) {
            if (cur->random) cur->next->random = cur->random->next;
            cur = cur->next->next;
        }
        Node* ans = head->next;
        cur = head;
        while (cur) {
            Node* nxt = cur->next;
            cur->next = cur->next->next;
            cur = cur->next;
            if (cur) nxt->next = cur->next;
        }
        return ans;
    }
};
