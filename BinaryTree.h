#pragma once
#include <functional>

namespace algds {

	template <class K, class I>
	class BinaryTree {
	private:
		struct node {
			K key = K();
			I value = I();
			node* parent = nullptr;
			node* left = nullptr;
			node* right = nullptr;
		};
		node* root = nullptr;
	private:
		node* succesor(node* x);
		node* find(const K& key);
	public:
		BinaryTree();
		~BinaryTree();
		void run(std::function<void(std::pair<K, I>)> func);
		const K& getMin()const;
		const K& getMax()const;
		void insert(const K& key, const I& value);
		I& search(const K& key);
		void erase(const K& key);
	};

	template <class K, class I>
	BinaryTree<K, I>::BinaryTree() {

	}

	template<class K, class I>
	BinaryTree<K, I>::~BinaryTree()
	{
		std::list<K> history;
		run([&history](std::pair<K, I> p)->void {
			history.push_back(p.first);
			});
		for (auto& el : history)
			erase(el);
	}

	template<class K, class I>
	void BinaryTree<K, I>::run(std::function<void(std::pair<K, I>)> func) {
		std::list<node*> list;
		list.push_back(this->root);
		while (!list.empty()) {
			node* p = list.back();
			func(std::make_pair(p->key, p->value));
			list.pop_back();
			if (p->left)
				list.push_back(p->left);
			if (p->right)
				list.push_back(p->right);
		}
	}

	template <class K, class I>
	const K& BinaryTree<K, I>::getMin()const {
		node* point = this->root;
		if (!point)
			throw std::out_of_range("root is empty");
		while (point->left)
			point = point->left;
		return point->key;
	}

	template <class K, class I>
	const K& BinaryTree<K, I>::getMax()const {
		node* point = this->root;
		if (!point)
			throw std::out_of_range("root is empty");
		while (point->right)
			point = point->right;
		return point->key;
	}

	template <class K, class I>
	void BinaryTree<K, I>::insert(const K& key, const I& value) {
		node* y = nullptr;
		node* x = this->root;
		while (x) {
			y = x;
			if (key < x->key)
				x = x->left;
			else
				x = x->right;
		}
		x = new node{ key,value,y };
		if (!this->root)
			this->root = x;
		else if (x->key < y->key)
			y->left = x;
		else
			y->right = x;
	}

	template<class K, class I>
	BinaryTree<K, I>::template node* BinaryTree<K, I>::succesor(BinaryTree<K, I>::node* x) {
		if (!x->left)
			return x;
		node* y = x->left;
		while (y->right)
			y = y->right;
	}

	template<class K, class I>
	BinaryTree<K, I>::template node* BinaryTree<K, I>::find(const K& key) {
		node* point = this->root;
		while (point && point->key != key)
		{
			if (key < point->key)
				point = point->left;
			else
				point = point->right;
		}
		return point;
	}

	template<class K, class I>
	I& BinaryTree<K, I>::search(const K& key) {
		node* point = find(key);
		if (!point)
			throw std::domain_error("not found");
		return point->value;
	}

	template<class K, class I>
	void BinaryTree<K, I>::erase(const K& key) {
		node* x = find(key);
		if (!x)
			throw std::domain_error("not found");
		node* y = x;
		node* s = nullptr;
		if (x->left && x->right) {
			y = succesor(x);
			x->key = y->key;
			x->value = y->value;
		}
		if (y->left)
			s = y->left;
		else
			s = y->right;
		if (y->parent == nullptr) {
			this->root = s;
		}
		else {
			if (s)
				s->parent = y->parent;
			if (y->parent->left == y)
				y->parent->left = s;
			else
				y->parent->right = s;
		}
		delete y;
	}
}