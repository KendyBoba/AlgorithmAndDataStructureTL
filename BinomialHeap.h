#pragma once
#include <vector>
#include <list>

namespace algds {

	template<class K>
	bool CompLess(K key1, K key2) {
		return key1 <= key2;
	}

	template<class K>
	bool CompMore(K key1, K key2) {
		return key1 >= key2;
	}

	template<class K, class I, bool (*Comp)(K, K)>
	class BinomialHeap {
	private:
		struct Node {
			K key = K();
			I item = I();
			unsigned short degree = 0;
			Node* child = nullptr;
			Node* sibling = nullptr;
		};
		Node* head = nullptr;
		bool (*comp)(K, K) = Comp;
	private:
		Node* UnionNode(Node* a, Node* b);
		BinomialHeap MakeMinHeap(const K& key, const I& item);
		Node* copyTree(Node* tree);
		void copyHeap(const BinomialHeap& heap);
		BinomialHeap merge(BinomialHeap& heap_first, BinomialHeap& heap_second);
		Node* findMinMax();
	public:
		BinomialHeap();
		~BinomialHeap();
		BinomialHeap(const BinomialHeap& heap);
		BinomialHeap(BinomialHeap&& heap);
		BinomialHeap& operator=(const BinomialHeap& heap);
		BinomialHeap& operator=(BinomialHeap&& heap);
		void Insert(const K& key, const I& item);
		void Union(const BinomialHeap& heap);
		std::pair<K, I> getMinMax();
		void removeMinMax();
	};

	template<class K, class I, bool (*Comp)(K, K)>
	BinomialHeap<K, I, Comp>::BinomialHeap() {

	}

	template<class K, class I, bool (*Comp)(K, K)>
	BinomialHeap<K, I, Comp>::~BinomialHeap() {
		if (!this->head)
			return;
		std::list<Node*> history;
		auto it = this->head;
		history.push_back(head);
		while (!history.empty()) {
			auto el = history.front();
			history.pop_front();
			auto it = el->child;
			while (it)
			{
				history.push_back(it);
				it = it->sibling;
			}
			delete el;
		}
		this->head = nullptr;
	}

	template<class K, class I, bool (*Comp)(K, K)>
	BinomialHeap<K, I, Comp>::BinomialHeap(const BinomialHeap& heap) {
		copyHeap(heap);
	}

	template<class K, class I, bool (*Comp)(K, K)>
	BinomialHeap<K, I, Comp>::BinomialHeap(BinomialHeap&& heap) {
		if (this == &heap)
			return;
		this->head = heap.head;
		heap.head = nullptr;
	}

	template<class K, class I, bool (*Comp)(K, K)>
	BinomialHeap<K, I, Comp>& BinomialHeap<K, I, Comp>::operator=(BinomialHeap&& heap) {
		if (this == &heap)
			return *this;
		this->head = heap.head;
		heap.head = nullptr;
		return *this;
	}

	template<class K, class I, bool (*Comp)(K, K)>
	BinomialHeap<K, I, Comp>& BinomialHeap<K, I, Comp>::operator=(const BinomialHeap& heap) {
		this->~BinomialHeap();
		copyHeap(heap);
		return *this;
	}

	template<class K, class I, bool (*Comp)(K, K)>
	typename BinomialHeap<K, I, Comp>::Node* BinomialHeap<K, I, Comp>::copyTree(Node* tree) {
		Node* res = nullptr;
		std::list<Node*> l;
		std::vector<Node*> arr_of_node;
		l.push_back(tree);
		while (!l.empty()) {
			auto el = l.front();
			l.pop_front();
			auto it = el->child;
			while (it)
			{
				l.push_back(it);
				it = it->sibling;
			}
			arr_of_node.push_back(new Node{ el->key,el->item,el->degree });
		}
		unsigned short sum_of_child = 1;
		for (unsigned short i = 0; i < arr_of_node.size(); ++i) {
			auto el = arr_of_node[i];
			if (!el->degree)
				continue;
			el->child = arr_of_node[sum_of_child];
			for (unsigned short j = sum_of_child; j < sum_of_child + el->degree - 1; ++j) {
				arr_of_node[j]->sibling = arr_of_node[j + 1];
			}
			sum_of_child += el->degree;
		}
		res = arr_of_node[0];
		return res;
	}

	template<class K, class I, bool (*Comp)(K, K)>
	void  BinomialHeap<K, I, Comp>::copyHeap(const  BinomialHeap& heap) {
		this->head = copyTree(heap.head);
		auto it = heap.head->sibling;
		auto nit = this->head;
		while (it) {
			nit->sibling = copyTree(it);
			it = it->sibling;
			nit = nit->sibling;
		}
	}

	template<class K, class I, bool (*Comp)(K, K)>
	typename BinomialHeap<K, I, Comp>::Node* BinomialHeap<K, I, Comp>::UnionNode(typename BinomialHeap<K, I, Comp>::Node* a, typename BinomialHeap<K, I, Comp>::Node* b) {
		if (a->degree != b->degree)
			return nullptr;
		if (comp(a->key, b->key)) {
			a->sibling = nullptr;
			b->sibling = a->child;
			a->child = b;
			++a->degree;
			return a;
		}
		else {
			b->sibling = nullptr;
			a->sibling = b->child;
			b->child = a;
			++b->degree;
			return b;
		}
	}

	template<class K, class I, bool (*Comp)(K, K)>
	BinomialHeap<K, I, Comp> BinomialHeap<K, I, Comp>::MakeMinHeap(const K& key, const I& item) {
		BinomialHeap res;
		res.head = new Node{ key,item,0,nullptr,nullptr };
		return res;
	}

	template<class K, class I, bool (*Comp)(K, K)>
	BinomialHeap<K, I, Comp> BinomialHeap<K, I, Comp>::merge(BinomialHeap<K, I, Comp>& heap_first, BinomialHeap<K, I, Comp>& heap_second) {
		BinomialHeap<K, I, Comp> res;
		auto it1 = heap_first.head;
		auto it2 = heap_second.head;
		if (it1->degree <= it2->degree) {
			res.head = copyTree(it1);
			res.head->sibling = copyTree(it2);
		}
		else {
			res.head = copyTree(it2);
			res.head->sibling = copyTree(it1);
		}
		it1 = it1->sibling;
		it2 = it2->sibling;
		auto it = res.head->sibling;
		while (it1 && it2) {
			if (comp(it1->degree, it2->degree)) {
				it->sibling = copyTree(it1);
				it->sibling->sibling = copyTree(it2);
			}
			else {
				it->sibling = copyTree(it2);
				it->sibling->sibling = copyTree(it1);
			}
			it = it->sibling->sibling;
		}
		it1 = (it1) ? it1 : it2;
		while (it1) {
			it->sibling = copyTree(it1);
			it1 = it1->sibling;
			it = it->sibling;
		}
		auto x = res.head;
		Node* prev_x = nullptr;
		auto next_x = x->sibling;
		while (next_x) {
			if (x->degree == next_x->degree && x == res.head) {
				auto next_sib = next_x->sibling;
				res.head = UnionNode(x, next_x);
				res.head->sibling = next_sib;
				x = res.head;
				next_x = res.head->sibling;
			}
			else if (x->degree == next_x->degree) {
				auto next_sib = next_x->sibling;
				prev_x->sibling = UnionNode(x, next_x);
				prev_x->sibling->sibling = next_sib;
				next_x = next_sib;
			}
			else {
				prev_x = x;
				x = next_x;
				next_x = next_x->sibling;
			}
		}
		return res;
	}

	template<class K, class I, bool (*Comp)(K, K)>
	typename BinomialHeap<K, I, Comp>::Node* BinomialHeap<K, I, Comp>::findMinMax() {
		auto it = head->sibling;
		Node* p_min_max = head;
		while (it) {
			if (comp(it->key, p_min_max->key))
				p_min_max = it;
			it = it->sibling;
		}
		return p_min_max;
	}

	template<class K, class I, bool (*Comp)(K, K)>
	void BinomialHeap<K, I, Comp>::Union(const BinomialHeap& heap) {
		if (!this->head) {
			*this = heap;
			return;
		}
		*this = this->merge(*this, heap);
	}

	template<class K, class I, bool (*Comp)(K, K)>
	void BinomialHeap<K, I, Comp>::Insert(const K& key, const I& item) {
		auto new_heap = MakeMinHeap(key, item);
		if (!this->head) {
			*this = new_heap;
			return;
		}
		*this = this->merge(*this, new_heap);
	}

	template<class K, class I, bool (*Comp)(K, K)>
	std::pair<K, I> BinomialHeap<K, I, Comp>::getMinMax() {
		auto it = findMinMax();
		return std::make_pair(it->key, it->item);
	}

	template<class K, class I, bool (*Comp)(K, K)>
	void BinomialHeap<K, I, Comp>::removeMinMax() {
		auto it = findMinMax();
		auto prev = head;
		if (prev != it) {
			while (prev->sibling != it) {
				prev = prev->sibling;
			}
		}
		prev->sibling = it->sibling;

		BinomialHeap temp_heap;
		auto child = it->child;
		prev = nullptr;
		while (child->sibling) {
			auto sib = child->sibling;
			child->sibling = prev;
			prev = child;
			child = sib;
		}
		child->sibling = prev;
		temp_heap.head = child;
		*this = merge(*this, temp_heap);
	}

}
