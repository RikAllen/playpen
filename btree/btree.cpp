// btree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <functional>
#include <assert.h>

template <class T>
class Tree {
public:
	static int num_nodes;
	struct Node {
		Node(T val) 
			: _val(val)
			, _less(nullptr)
			, _more(nullptr) {
			num_nodes++;
			printf("Create node - %d. Num nodes %d\n", _val, num_nodes);
		}
		~Node() {
			num_nodes--;
			printf("Delete node - %d. Num nodes %d\n", _val, num_nodes);
		}
		T _val;
		std::unique_ptr<Node> _less;
		std::unique_ptr<Node> _more;
	};
private:
	std::unique_ptr<Node> _root;

	void _traverse(std::unique_ptr<Node>& node, std::function<void(Node*)> fn) {
		if (node) {
			_traverse(node->_less, fn);
			fn(node.get());
			_traverse(node->_more, fn);
		}
	}
	// Recurse - Node** since we will change the last Node* we find that is nullptr - change it to point to the new node.
	bool _insert(T val, std::unique_ptr<Node>* node) {
		if (!*node) {
			*node = std::make_unique<Node>(val);
			return true;
		}
		else if (val < (*node)->_val) {
			return _insert(val, &((*node)->_less));
		}
		else if (val > (*node)->_val) {
			return _insert(val, &((*node)->_more));
		}
		return false;
	}

	// Using Node** since we use this function to support Erase - which needs to be able to modify the pointer to the node
	std::unique_ptr<Node>* _find(int val, std::unique_ptr<Node>* p_node) {
		if (*p_node) {
			if ((*p_node)->_val == val) {
				return p_node;
			} 
			else if (val < (*p_node)->_val) {
				return _find(val, &(*p_node)->_less);
			}
			else {
				return _find(val, &(*p_node)->_more);
			}
		}
		return nullptr;
	}


public:
	Tree(): _root(nullptr) {}
	bool find(T val) {

		if (_find(val, &_root) != NULL)	{
			return true;
		}
		return false;
	}
	// Delete a node - return false if it does not exist
	bool erase(T val) {
		printf("erase %d\n", val);
		std::unique_ptr<Node>* ppNode = _find(val, &_root);
		if (ppNode) {
			std::unique_ptr<Node>& node = *ppNode;
			std::unique_ptr<Node>& more = node->_more;
			std::unique_ptr<Node>& less = node->_less;

			if (less) {
				node = std::move(less);
				if (more) {
				// Both children exist - need to recurse until there is somewhere to put the more side
					node = std::move(less);
					Node* pTmp = node.get(); // Use raw pointers while iterativly digging down - can't assign to uniques and don't need to own anything
					while (pTmp->_more)
					{
						pTmp = pTmp->_more.get();
					}
					pTmp->_more = std::move(more);
				}
			} 
			else if (more) {
				// One child - replace this with it's child
				node = std::move(more);
			} 
			else {
				// No children - simple delete of one node
				node.reset();
			}
			return true;
		}
		return false;
	}
	// Insert a node - return true on success. False on failure (e.g. duplicate value);
	bool insert(T val) { 
		printf("Insert %d\n", val); 
		return _insert(val, &_root); 
	}
	void traverse(std::function<void(Node*)> fn) {
		_traverse(_root, fn); printf("\n");
	}
	// Trash the tree
	void empty() {
		_root.reset();
	}
};
int Tree<int>::num_nodes{ 0 };
/////////////////////////
char* itoa(int i, char* str)
{
	char* ptr = str;
	unsigned int foo;

	if (i < 0)
	{
		*ptr++ = '-';
		foo = -i;
	}
	else {
		foo = i;
	}
	char reversed[16] = { 0 };
	int index = 0;
	do {
		reversed[index++] = (foo % 10) + '0';
		foo /= 10;
	} while (foo);
	while (index--) {
		*ptr++ = reversed[index];
	}
	*ptr = 0;
	return str;
}
////////////////////////////
void test_itoa()
{
	char buffer[16];
	int i = 0;
	printf("%d is %s\n", i, itoa(i, buffer));

	i = 25;
	printf("%d is %s\n", i, itoa(i, buffer));

	i = -1;
	printf("%d is %s\n", i, itoa(i, buffer));

	i = std::numeric_limits<int>::min();
	printf("%d is %s\n", i, itoa(i, buffer));
	i = std::numeric_limits<int>::max();
	printf("%d is %s\n", i, itoa(i, buffer));
}

void test_btree()
{
	Tree<int> tree;

	auto fn = [](Tree<int>::Node *node) {printf("%d\n", node->_val); };

	assert(tree.insert(5) && "Failed to insert node");
	tree.traverse(fn);
	assert(!tree.find(0) && "Find returned true for a node that should node exist");
	assert(tree.find(5) && "Failed to find a node we expected");

	assert(tree.insert(15) && "Failed to insert node");
	tree.traverse(fn);
	assert(tree.insert(1) && "Failed to insert node");
	tree.traverse(fn);
	assert(!tree.insert(1) && "Insert of duplicate node should give false result");
	assert(tree.insert(11) && "Failed to insert node");
	tree.traverse(fn);
	assert(tree.insert(2) && "Failed to insert node");
	tree.traverse(fn);

	tree.erase(17); tree.traverse(fn); // Not in tree
	for (int i = 0; i < 20; i++) tree.find(i);

	assert(tree.erase(11) && "Failed to erase node");
	tree.traverse(fn);
	assert(!tree.erase(11) && "Erase of non existant node returned true");
	assert(tree.erase(15) && "Failed to erase node");
	tree.traverse(fn);
	assert(tree.erase(2) && "Failed to erase node");
	tree.traverse(fn);
	assert(tree.erase(1) && "Failed to erase node");
	tree.traverse(fn);
	assert(tree.erase(5) && "Failed to erase node");
	tree.traverse(fn);

	printf("Add 200 nodes");
	for (int i = 0; i < 200; i++) tree.insert(std::rand() % 1024);
	tree.traverse(fn);
	printf("Empty\n");
	tree.empty();

	printf("Done empty\n");
}

int main()
{
	test_itoa();
	test_btree();
}
