#ifndef HBTREE
#define HBTREE
#include <queue>
#include <functional>

namespace algds {

    template<class Key, class Value, int t>
    class BTree {
    private:
        struct BNode {
            unsigned int len = 0;
            std::pair<Key, Value> elements[2 * t - 1];
            BNode* childs[2 * t];
            bool is_leafe = true;
        };
    private:
        class pcont {
        private:
            BNode* pd = nullptr;
            unsigned int index = 0;
        public:
            pcont(BNode* pd, unsigned int i);
            pcont();
            pcont(const pcont& obj) = delete;
            pcont(pcont&& obj) noexcept;
            pcont& operator=(const pcont& obj) = delete;
            pcont& operator=(pcont&& obj) noexcept;
            Value& operator*();
            bool empty()const;
            friend bool operator==(const pcont& obj1, const pcont& obj2);
            friend bool operator!=(const pcont& obj1, const pcont& obj2);
        };
    private:
        BNode* root = nullptr;
    private:
        void Split(BNode* parent, BNode* node);
        void insertNonFull(BNode* node, const Key& key, const Value& val);
        void deleteFromNode(BNode* node, unsigned int pos);
        bool merge(BNode* parent, unsigned int pos);
        bool overbalance(BNode* parent, unsigned int pos);
        void run(std::function<void(BNode*)> func);
    public:
        BTree();
        BTree(BTree& btree);
        BTree(BTree&& btree) noexcept;
        BTree& operator=(BTree& btree);
        BTree& operator=(BTree&& btree) noexcept;
        ~BTree();
        void Delete(Key key);
        void insert(const Key& key, const Value& val);
        pcont Search(const Key& key) const;
        bool contains(const Key& key) const;
    };

    template<class Key, class Value, int t>
    BTree<Key, Value, t>::pcont::pcont(BNode* pd, unsigned int i) :pd(pd), index(i) {}

    template<class Key, class Value, int t>
    BTree<Key, Value, t>::pcont::pcont() {}

    template<class Key, class Value, int t>
    BTree<Key, Value, t>::pcont::pcont(pcont&& obj) noexcept {
        this->pd = obj.pd;
        obj.pd = nullptr;
    }

    template<class Key, class Value, int t>
    typename BTree<Key, Value, t>::pcont& BTree<Key, Value, t>::pcont::operator=(typename BTree<Key, Value, t>::pcont&& obj)noexcept {
        if (&obj == this)
            return *this;
        this->pd = obj.pd;
        obj.pd = nullptr;
    }

    template<class Key, class Value, int t>
    Value& BTree<Key, Value, t>::pcont::operator*() {
        if (!pd)
            throw std::runtime_error("pcont was be empty");
        if (index >= pd->len)
            throw std::out_of_range("invalid index");
        return pd->elements[index].second;
    }

    template<class Key, class Value, int t>
    bool BTree<Key, Value, t>::pcont::empty()const {
        return (pd == nullptr);
    }

    template<class Key, class Value, int t>
    bool operator==(const typename BTree<Key, Value, t>::pcont& obj1, const typename BTree<Key, Value, t>::pcont& obj2) {
        return obj1.pd == obj2.pd;
    }

    template<class Key, class Value, int t>
    bool operator!=(const typename BTree<Key, Value, t>::pcont& obj1, const typename BTree<Key, Value, t>::pcont& obj2) {
        return !(obj1 == obj2);
    };

    template<class Key, class Value, int t>
    typename BTree<Key, Value, t>::pcont BTree<Key, Value, t>::Search(const Key& key)const {
        auto it = this->root;
        while (it) {
            unsigned int i;
            for (i = 0; i < it->len && key > it->elements[i].first; ++i) {}
            if (key == it->elements[i].first)
                return pcont(it, i);
            it = it->childs[i];
        }
        return pcont();
    }

    template<class Key, class Value, int t>
    bool BTree<Key, Value, t>::contains(const Key& key) const {
        return (Search(key).empty()) ? false : true;
    }

    template<class Key, class Value, int t>
    void BTree<Key, Value, t>::Split(BNode* parent, BNode* node) {
        BNode* right_node = new BNode{};
        right_node->is_leafe = node->is_leafe;
        right_node->len = t - 1;
        for (unsigned int i = 0; i < right_node->len; ++i) {
            right_node->elements[i] = node->elements[i + t];
        }
        if (!right_node->is_leafe) {
            for (unsigned int i = 0; i < t; ++i) {
                right_node->childs[i] = node->childs[i + t];
            }
        }
        node->len = t - 1;
        int i = parent->len - 1;
        if (i < 0) {
            parent->elements[0] = node->elements[t - 1];
            parent->len = 1;
            parent->childs[i + 2] = right_node;
            parent->childs[i + 1] = node;
        }
        else {
            ++parent->len;
            while (i >= 0 && node->elements[t - 1] < parent->elements[i]) {
                parent->elements[i + 1] = parent->elements[i];
                --i;
            }
            parent->elements[i + 1] = node->elements[t - 1];
            for (unsigned int j = parent->len; j > i + 1; --j) {
                parent->childs[j + 1] = parent->childs[j];
            }
            parent->childs[i + 2] = right_node;
            parent->childs[i + 1] = node;
        }
    }

    template<class Key, class Value, int t>
    void BTree<Key, Value, t>::insertNonFull(BNode* node, const Key& key, const Value& val) {
        if (!node->len) {
            node->elements[0] = std::make_pair(key, val);
            ++node->len;
            return;
        }
        int i = node->len - 1;
        while (i >= 0 && key < node->elements[i].first) {
            node->elements[i + 1] = node->elements[i];
            --i;
        }
        node->elements[i + 1] = std::make_pair(key, val);
        ++node->len;
    }

    template<class Key, class Value, int t>
    void BTree<Key, Value, t>::insert(const Key& key, const Value& val) {
        if (contains(key))
            return;
        if (!this->root) {
            root = new BNode{};
            insertNonFull(this->root, key, val);
            return;
        }
        BNode* it = root;
        BNode* prev = nullptr;
        if (it->len >= 2 * t - 1) {
            prev = new BNode{};
            prev->is_leafe = false;
            Split(prev, it);
            this->root = prev;
            it = root;
        }
        while (!it->is_leafe) {
            int i = it->len - 1;
            while (i >= 0 && key < it->elements[i].first) { --i; };
            if (it->childs[i + 1]->len >= 2 * t - 1) {
                Split(it, it->childs[i + 1]);
                continue;
            }
            prev = it;
            it = it->childs[i + 1];
        }
        if (it->len >= 2 * t - 1) {
            Split(prev, it);
            insertNonFull(it, key, val);
        }
        else {
            insertNonFull(it, key, val);
        }
    }

    template<class Key, class Value, int t>
    void BTree<Key, Value, t>::deleteFromNode(BNode* node, unsigned int pos) {
        for (unsigned int i = pos; i < node->len; ++i)
            node->elements[i] = node->elements[i + 1];
        for (unsigned int i = pos; i <= node->len; ++i)
            node->childs[i] = node->childs[i + 1];
        --node->len;
        if (node == root && !node->len) {
            delete root;
            root = nullptr;
        }
    }

    template<class Key, class Value, int t>
    bool BTree<Key, Value, t>::merge(BNode* parent, unsigned int pos) {
        BNode* left = nullptr;
        BNode* right = parent->childs[pos + 1];
        BNode* mid = parent->childs[pos];
        bool res = false;
        if (pos > 0)
            left = parent->childs[pos - 1];
        if (right && right->len + mid->len < 2 * t - 1) {
            mid->elements[mid->len] = parent->elements[pos];
            unsigned int i;
            for (i = 0; i < right->len; ++i) {
                mid->elements[i + mid->len + 1] = right->elements[i];
                mid->childs[i + mid->len + 1] = right->childs[i];
            }
            mid->childs[i + mid->len + 1] = right->childs[right->len];
            for (i = pos; i < parent->len; ++i)
                parent->elements[i] = parent->elements[i + 1];
            for (i = pos + 1; i <= parent->len; ++i)
                parent->childs[i] = parent->childs[i + 1];
            --parent->len;
            mid->len = mid->len + right->len + 1;
            res = true;
        }
        else if (left && left->len + mid->len < 2 * t - 1) {
            unsigned int i;
            for (i = left->len + 1; i < mid->len + left->len + 1; ++i) {
                mid->elements[i] = mid->elements[i - left->len - 1];
            }
            for (i = 0; i < left->len; ++i) {
                mid->elements[i] = left->elements[i];
            }
            pos = pos - 1;
            mid->elements[left->len] = parent->elements[pos];
            mid->len = mid->len + left->len + 1;
            for (i = pos; i < parent->len; ++i)
                parent->elements[i] = parent->elements[i + 1];
            for (i = pos; i <= parent->len; ++i)
                parent->childs[i] = parent->childs[i + 1];
            --parent->len;
            res = true;
        }
        if (parent == root && !parent->len) {
            delete root;
            root = mid;
        }
        return res;
    }

    template<class Key, class Value, int t>
    bool BTree<Key, Value, t>::overbalance(BNode* parent, unsigned int pos) {
        BNode* left = nullptr;
        BNode* right = parent->childs[pos + 1];
        BNode* mid = parent->childs[pos];
        if (pos > 0)
            left = parent->childs[pos - 1];
        if (right && right->len >= t) {
            mid->elements[mid->len] = parent->elements[pos];
            mid->childs[mid->len + 1] = right->childs[0];
            parent->elements[pos] = right->elements[0];
            ++mid->len;
            deleteFromNode(right, 0);
            return true;
        }
        if (left && left->len >= t) {
            for (int i = mid->len; i >= 0; --i) {
                mid->elements[i + 1] = mid->elements[i];
            }
            for (int i = mid->len + 1; i >= 0; --i) {
                mid->childs[i + 1] = mid->childs[i];
            }
            mid->elements[0] = parent->elements[pos];
            parent->elements[pos] = left->elements[left->len - 1];
            mid->childs[0] = left->childs[left->len + 1];
            left->childs[left->len + 1] = nullptr;
            ++mid->len;
            --left->len;
            return true;
        }
        return false;
    }

    template<class Key, class Value, int t>
    void BTree<Key, Value, t>::Delete(Key key) {
        if (!contains(key))
            return;
        BNode* it = this->root;
        BNode* prev_node = this->root;
        unsigned prev_index = 0;
        BNode* next = nullptr;
        while (it) {
            unsigned int i;
            for (i = 0; i < it->len && key > it->elements[i].first; ++i) {}
            next = it->childs[i];
            if (key == it->elements[i].first) {
                if (it->is_leafe) {
                    deleteFromNode(it, i);
                    return;
                }
                else {
                    if (it->childs[i + 1]->len >= t) {
                        BNode* right = it->childs[i + 1];
                        while (!right->is_leafe)
                        {
                            right = right->childs[0];
                        }
                        it->elements[i] = right->elements[0];
                        next = it->childs[i + 1];
                    }
                    else if (it->childs[i]->len >= t) {
                        BNode* left = it->childs[i];
                        while (!left->is_leafe)
                        {
                            left = left->childs[left->len];
                        }
                        it->elements[i] = left->elements[left->len - 1];
                    }
                    else {
                        merge(it, i);
                        if (!it->len) {
                            delete it;
                            prev_node->childs[prev_index] = next;
                        }
                    }
                    key = it->elements[i].first;
                }
            }
            else {
                if (it->childs[i]->len < t) {
                    (overbalance(it, i)) ? false : merge(it, i);
                    if (!it->len) {
                        delete it;
                        prev_node->childs[prev_index] = next;
                    }
                }
            }
            prev_index = i;
            prev_node = it;
            it = next;
        }
    }

    template<class Key, class Value, int t>
    void BTree<Key, Value, t>::run(std::function<void(BNode*)> func) {
        std::queue<BNode*> q;
        if (!root)
            return;
        q.push(root);
        while (!q.empty())
        {
            BNode* top = q.front();
            q.pop();
            for (unsigned int i = 0; i <= top->len; ++i) {
                if (!top->is_leafe)
                    q.push(top->childs[i]);
            }
            func(top);
        }
    }

    template<class Key, class Value, int t>
    BTree<Key, Value, t>::BTree() {}

    template<class Key, class Value, int t>
    BTree<Key, Value, t>::BTree(BTree& btree) {
        btree.run([this](BNode* pelem)->void {
            for (unsigned int i = 0; i < pelem->len; ++i)
                this->insert(pelem->elements[i].first, pelem->elements[i].second);
            });
    }

    template<class Key, class Value, int t>
    BTree<Key, Value, t>::BTree(BTree&& btree)noexcept {
        this->root = btree.root;
        btree.root = nullptr;
    }

    template<class Key, class Value, int t>
    BTree<Key, Value, t>& BTree<Key, Value, t>::operator=(BTree& btree) {
        if (&btree == this)
            return *this;
        this->~BTree();
        btree.run([this](BNode* pelem)->void {
            for (unsigned int i = 0; i < pelem->len; ++i)
                this->insert(pelem->elements[i].first, pelem->elements[i].second);
            });
        return *this;
    }

    template<class Key, class Value, int t>
    BTree<Key, Value, t>& BTree<Key, Value, t>::operator=(BTree&& btree) noexcept {
        if (&btree == this)
            return *this;
        this->~BTree();
        this->root = btree.root;
        btree.root = nullptr;
        return *this;
    }

    template<class Key, class Value, int t>
    BTree<Key, Value, t>::~BTree() {
        run([](BNode* pelem)->void {
            delete pelem;
            });
        this->root = nullptr;
    }
}
#endif