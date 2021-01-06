// btree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <functional>


template <class T>
class Tree {
public:
	struct Node {
		Node(T val) 
			: _val(val)
			, _less(nullptr)
			, _more(nullptr)
		{}

		T _val;
		Node* _less;
		Node* _more;
	};
private:
	Node* _root;

	void _traverse(Node* node, std::function<void(Node*)> fn)
	{
		if (node) {
			_traverse(node->_less, fn);
			fn(node);
			_traverse(node->_more, fn);
		}
	}
	void _insert(T val, Node** node)
	{
		if (!*node)
		{
			*node = new Node(val);
		}
		else if (val < (*node)->_val)
		{
			_insert(val, &((*node)->_less));
		}
		else
		{
			_insert(val, &((*node)->_more));
		}
		
	}
	Node** _find(int val, Node** p_node)
	{
		if (*p_node)
		{
			if ((*p_node)->_val == val)
			{
				return p_node;
			} 
			else if (val < (*p_node)->_val)
			{
				return _find(val, &(*p_node)->_less);
			}
			else
			{
				return _find(val, &(*p_node)->_more);
			}
		}
		return nullptr;
	}


public:
	Tree(): _root(nullptr) {}
	void find(T val) {

		if (_find(val, &_root) != NULL)
		{
			printf("Found %d\n", val);
		}
		else
		{
			printf("Not found %d\n", val);
		}
	}
	void erase(T val)
	{
		printf("erase %d\n", val);
		Node** ppNode = _find(val, &_root);
		if (ppNode)
		{
			Node* pNode = *ppNode;
			// Favour left - prone to tree imbalance
			if (pNode->_less)
			{
				*ppNode = pNode->_less;
			}
			else
			{
				*ppNode = pNode->_more;
			}
			delete pNode;
		}
	}
	void insert(T val) { 
		printf("Insert %d\n", val); 
		_insert(val, &_root); 
	}
	void traverse(std::function<void(Node*)> fn) {
		_traverse(_root, fn); printf("\n");
	}

};

int main()
{
    std::cout << "Hello World!\n";
	Tree<int> tree;

	auto fn = [](Tree<int>::Node *node) {printf("%d\n", node->_val); };

	tree.insert(5);	tree.traverse(fn);
	tree.find(0);
	tree.find(5);

	tree.insert(15); tree.traverse(fn);
	tree.insert(1); tree.traverse(fn);
	tree.insert(11); tree.traverse(fn);
	tree.insert(2); tree.traverse(fn);

	tree.erase(17); tree.traverse(fn); // Not in tree
	for (int i = 0; i < 20; i++) tree.find(i);

	tree.erase(11); tree.traverse(fn);
	tree.erase(15); tree.traverse(fn);
	tree.erase(2); tree.traverse(fn);
	tree.erase(1); tree.traverse(fn);
	tree.erase(5); tree.traverse(fn);

	for (int i = 0; i < 20; i++) tree.find(i);
}
