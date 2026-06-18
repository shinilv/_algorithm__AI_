/*
题目要求：实现LRUCache缓存： LRUCache 是一种在容量满时优先淘汰最久未使用数据的缓存机制。

思路：使用双向链表去维护这个Cache， 如果get，先判断链表中是否存在这个key，不存在返回-1
存在则需要把这个节点移动到链表头部
如果put， 首先判断链表中是否存在这个key， 如果存在， 需要把这个节点的val重新赋值
然后把这个节点移动到头部，如果不存在，new一个Node， 头插加入链表， 接着判断链表节点个数是否大于最大容量，
如果大于，执行链表尾部删除。
可以使用数据结构unordered_map<int(key), Node*>, 这样可以快速查找key值对应的Node

为了完成以上功能，初始化链表的时候需要一个哨兵节点
Node* getNode(int key) 这个函数负责查找key对应的Node， 不存在则返回nullptr。需要把这个节点先删除，然后再头插
void remove(Node* x)  删除节点x， 这里删除节点x但不释放这个节点。
void pushFront(Node* x)  头插
*/
#include <unordered_map>
class LRUCache {
private:
    struct Node {
        int key;
        int val;
        Node* next;
        Node* prev;
        Node(int key = 0, int val = 0) : key(key), val(val) {}
    };
    Node* head_tail;
    int _capacity;
    std::unordered_map<int, Node*> key_to_node;
    void remove(Node* x) {
        x->prev->next = x->next;
        x->next->prev = x->prev;
    }
    void pushFront(Node* x) {
        x->next = head_tail->next;
        x->prev = head_tail;
        head_tail->next->prev = x;
        head_tail->next = x;
    }
    Node* getNode(int key) {
        auto it = key_to_node.find(key);
        if (it == key_to_node.end()) {
            return nullptr;
        }
        Node* x = it->second;
        remove(x);
        pushFront(x);
        return x;
    }
public:
    LRUCache(int capacity) : head_tail(new Node(0, 0)), _capacity(capacity) {
        head_tail->next = head_tail;
        head_tail->prev = head_tail;
    }

    int get(int key) {
        Node* temp = getNode(key);
        if (temp == nullptr) {
            return -1;
        }
        return temp->val;
    }

    void put(int key, int value) {
        Node* temp = getNode(key);
        if (temp) {
            temp->val = value;
            return;
        }
        temp = key_to_node[key] = new Node(key, value);
        pushFront(temp);
        if (key_to_node.size() > _capacity) {
            Node* x = head_tail->prev;
            key_to_node.erase(x->key);
            remove(x);
            delete x;
        }
    }
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */
