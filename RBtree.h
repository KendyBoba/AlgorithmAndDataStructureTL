#pragma once
#include <functional>
#include <list>

template<class K,class I>
class RBtree {
public:
	struct node
	{
		I value = I();
		K key = K();
		node *left = nullptr;
		node *right = nullptr;
		node *parent = nullptr;
		bool is_red = true;
	};
	RBtree();
	~RBtree();
private:
	node* root = nullptr;
	node* find(const K& key);
	node* successor(node* x);
	void LeftRotate(node* x);
	void RightRotate(node* x);
	void InsertFixUp(node* x);
	void DeleteNode(node* x);
public:
	void erase(const K& elem);
	void insert(const K& key, const I& elem);
	I& search(const K& key);
	void run(std::function<void(std::pair<K, I>)> func);
};

template<class K, class I> 
RBtree<K, I>::RBtree() {
	
};

template<class K, class I>
RBtree<K, I>::~RBtree() {
	std::list<K> history;
	run([&history](std::pair<K, I> p)->void {
		history.push_back(p.first);
		});
	for (auto& el : history)
		erase(el);
};

template<class K, class I>
void RBtree<K, I>::run(std::function<void(std::pair<K, I>)> func) {
	std::list<node*> list;
	list.push_back(this->root);
	while (!list.empty()) {
		node* p = list.back();
		std::pair<K, I> elem = std::make_pair(p->key, p->value);
		func(elem);
		list.pop_back();
		if (p->left)
			list.push_back(p->left);
		if (p->right)
			list.push_back(p->right);
	}
}

template<class K, class I>
I& RBtree<K, I>::search(const K& key) {
	node* find_elem = find(key);
	if (!find_elem)
		throw std::domain_error("not found");
	return find_elem->value;
};

template<class K, class I>
RBtree<K, I>::template node* RBtree<K, I>::find(const K& key) {
	node* point = this->root;
	while (point && point->key != key)
	{
		if (key < point->key)
			point = point->left;
		else
			point = point->right;
	}
	return point;
};

template<class K, class I> 
RBtree<K, I>::template node* RBtree<K, I>::successor(RBtree<K, I>::node* x) {
	if (!x->left)
		return x;
	node* y = x->left;
	while (y->right != nullptr)
		y = y->right;
	return y;
}

template<class K, class I> 
void RBtree<K, I>::LeftRotate(RBtree<K, I>::node* x) {
	node* y = x->right;
	if (x->parent != nullptr && x->parent->right == x)
		x->parent->right = y;
	else if (x->parent != nullptr && x->parent->left == x)
		x->parent->left = y;
	x->right = y->left;
	if (y->left)
		y->left->parent = x;
	y->left = x;
	y->parent = x->parent;
	x->parent = y;
	if (x == this->root)
		this->root = y;
}

template<class K, class I>
void RBtree<K, I>::RightRotate(RBtree<K, I>::node* x) {
	node* y = x->left;
	if (x->parent != nullptr && x->parent->right == x)
		x->parent->right = y;
	else if (x->parent != nullptr && x->parent->left == x)
		x->parent->left = y;
	x->left = y->right;
	if (y->right)
		y->right->parent = x;
	y->right = x;
	y->parent = x->parent;
	x->parent = y;
	if (x == this->root)
		this->root = y;
}

template<class K, class I>
void RBtree<K, I>::InsertFixUp(RBtree<K, I>::node* x) {
	while (x->parent && x->parent->is_red) {
		node* ancle = nullptr;
		node* parent = x->parent;
		node* grandfather = x->parent->parent;
		if (grandfather->right == parent) {
			ancle = grandfather->left;
			if (ancle && ancle->is_red) {
				parent->is_red = false;
				ancle->is_red = false;
				grandfather->is_red = true;
				x = grandfather;
			}
			else {
				grandfather->is_red = true;
				parent->is_red = false;
				if (parent->left == x) {
					x->is_red = false;
					parent->is_red = true;
					RightRotate(parent);
				}
				LeftRotate(grandfather);
			}
		}
		else {
			ancle = grandfather->right;
			if (ancle && ancle->is_red) {
				parent->is_red = false;
				ancle->is_red = false;
				grandfather->is_red = true;
				x = grandfather;
			}
			else {
				grandfather->is_red = true;
				parent->is_red = false;
				if (parent->right == x) {
					x->is_red = false;
					parent->is_red = true;
					LeftRotate(parent);
				}
				RightRotate(grandfather);
			}
		}
	}
	this->root->is_red = false;
}

template<class K, class I>
void RBtree<K, I>::insert(const K& key,const I& elem) {
	node* p = nullptr;
	node* x = this->root;
	if (!x) {
		this->root = new node{ elem,key,nullptr,nullptr,nullptr,false };
		return;
	}
	while (x != nullptr) {
		p = x;
		if (key < x->key)
			x = x->left;
		else if (key == x->key)
			return;
		else
			x = x->right;
	}
	if (key < p->key) {
		p->left = new node{ elem,key,nullptr,nullptr,p,true };
		InsertFixUp(p->left);
	}
	else {
		p->right = new node{ elem,key,nullptr,nullptr,p,true };
		InsertFixUp(p->right);
	}
}

template<class K, class I>
void RBtree<K, I>::DeleteNode(RBtree<K, I>::node* x) {
	if (x->parent && x->parent->left == x)
		x->parent->left = nullptr;
	else if (x->parent && x->parent->right == x)
		x->parent->right = nullptr;
	delete x;
}

template<class K, class I>
void RBtree<K, I>::erase(const K& elem) {
	node* z = find(elem);
	if (!z)
		throw std::domain_error("not found");
	node* y = nullptr;
	if (z->left && z->right) {
		y = successor(z);
		z->value = y->value;
		z->key = y->key;
	}
	else {
		y = z;
	}
	if (y->is_red) {
		DeleteNode(y);
	}
	else if (!y->is_red && (y->left || y->right)) {
		node* x = y->left;
		if (y->right)
			x = y->right;
		x->parent = y->parent;

		if (y->parent && y->parent->left == y)
			y->parent->left = x;
		else if (y->parent && y->parent->right == y)
			y->parent->right = x;
		else
			this->root = x;
		x->is_red = false;
		DeleteNode(y);
	}
	else {
		node* pointer = y;
		while (!pointer->is_red && pointer != this->root) {
			node* parent = pointer->parent;
			if (parent->left == pointer) {
				node* brother = parent->right;
				if (!parent->is_red && !brother->is_red && !brother->left && !brother->right) {
					brother->is_red = true;
					pointer = pointer->parent;
				}
				else if (parent->is_red && brother->right) {
					brother->is_red = true;
					parent->is_red = false;
					brother->right->is_red = false;
					LeftRotate(parent);
				}
				else if (parent->is_red && !brother->left && !brother->right) {
					parent->is_red = false;
					brother->is_red = true;
				}
				else if (!parent->is_red && brother->is_red) {
					if (brother->left->right)
						brother->left->right->is_red = false;
					else {
						brother->is_red = false;
						brother->right->is_red = true;
					}
					RightRotate(brother);
					LeftRotate(parent);
				}
				else if (!parent->is_red && !brother->is_red && brother->left) {
					brother->left->is_red = false;
					RightRotate(brother);
					LeftRotate(parent);
				}
				else {
					throw std::logic_error("unexpected event");
				}
			}
			else {
				node* brother = parent->left;
				if (!parent->is_red && !brother->is_red && !brother->left && !brother->right) {
					brother->is_red = true;
					pointer = pointer->parent;
				}
				else if (parent->is_red && brother->left) {
					brother->is_red = true;
					parent->is_red = false;
					brother->left->is_red = false;
					RightRotate(parent);
				}
				else if (parent->is_red && !brother->left && !brother->right) {
					parent->is_red = false;
					brother->is_red = true;
				}
				else if (!parent->is_red && brother->is_red) {
					if (brother->right->left)
						brother->right->left->is_red = false;
					else {
						brother->is_red = false;
						brother->left->is_red = true;
					}
					LeftRotate(brother);
					RightRotate(parent);
				}
				else if (!parent->is_red && !brother->is_red && brother->right) {
					brother->right->is_red = false;
					LeftRotate(brother);
					RightRotate(parent);
				}
				else {
					throw std::logic_error("unexpected event");
				}
			}
			pointer = this->root;
		}
		DeleteNode(y);
		this->root->is_red = false;
	}
}