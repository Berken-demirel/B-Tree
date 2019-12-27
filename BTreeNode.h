#pragma once

#include <assert.h>
#include <queue> 

// --------------Enumarators for easy use ---------------
enum E_CHILD
{
	CHILD_L = 0, CHILD_M = 1, CHILD_R = 2
};

enum E_VALUE
{
	VALUE_L = 0, VALUE_R = 1,
};

// BTreeNode class. TKey represents the StudentID which search and insert are based on. TValue is index of Student.
template <typename TKey, typename TValue>
class BTreeNode
{
public:

	using NodeType = BTreeNode<TKey, TValue>;
	using NodeTypePtr = NodeType * ;

	struct KeyValueInfo
	{
		KeyValueInfo() : is_available{false}
		{
		}
		TKey	key;			//Key for an item value
		TValue	value;			//Item value itself
		bool is_available;	//If true we have a key-value pair at this node, if not ignore that node
	};

	BTreeNode()
	{
		m_Children[CHILD_L] = m_Children[CHILD_M] = m_Children[CHILD_R] = 0;
	}
	// Function which returns the number of values in that node
	size_t ValueCount() const
	{
		size_t nRet = 0;

		nRet += m_values[VALUE_L].is_available ? 1 : 0;
		nRet += m_values[VALUE_R].is_available ? 1 : 0;

		return nRet;
	}
	// Function returns the number of children which that node have 
	size_t ChildCount() const
	{
		size_t nRet = 0;

		nRet += m_Children[CHILD_L] ? 1 : 0;
		nRet += m_Children[CHILD_M] ? 1 : 0;
		nRet += m_Children[CHILD_R] ? 1 : 0;
		
		return nRet;
	}
	// InsertValue function for modifying a node with Key and Value.
	void InsertValue(TKey key, const TValue &value)
	{
		switch (ValueCount())
		{
			case 0:
			{
				m_values[VALUE_L].key = key;
				m_values[VALUE_L].value = value;
				m_values[VALUE_L].is_available = true;
				break;
			}
			case 1:
			{
				m_values[VALUE_R].key = key;
				m_values[VALUE_R].value = value;
				m_values[VALUE_R].is_available = true;

				if (m_values[VALUE_R].key < m_values[VALUE_L].key)
				{
					std::swap(m_values[VALUE_L].value, m_values[VALUE_R].value);
					std::swap(m_values[VALUE_L].key, m_values[VALUE_R].key);
				}
				break;
			}
			case 2:
			{
				assert(false);
				return;
			}
		}
	}

	void InsertChildren(NodeTypePtr pChild1, NodeTypePtr pChild2)
	{
		if (pChild2->m_values[VALUE_L].key < pChild1->m_values[VALUE_L].key) //Swap the two nodes according to values
		{
			std::swap(pChild1, pChild2);
		}

		switch (ValueCount())
		{
			case 1: // The case for parent is the root of Tree
			{
				m_Children[CHILD_L] = pChild1;
				m_Children[CHILD_R] = pChild2;
				m_Children[CHILD_M] = nullptr;
				return;
			}
			case 2: // The case which parent has just one value
			{
				if (this->m_values[VALUE_L].key < pChild1->m_values[VALUE_L].key)
				{
					m_Children[CHILD_M] = pChild1;
					m_Children[CHILD_R] = pChild2;
				}
				else
				{
					m_Children[CHILD_M] = pChild2;
					m_Children[CHILD_L] = pChild1;
				}
				return;
			}
		}
	}

	// InsertValue function for modifying a node with Key and Value.
	void OverwriteLeft(TKey key, const TValue &value)
	{
		m_values[VALUE_L].key = key;
		m_values[VALUE_L].value = value;
		m_values[VALUE_L].is_available = true;
	}

	void OverwriteRight_Children(NodeTypePtr pChild)
	{
		m_Children[CHILD_R] = pChild;
	}

	void OverwriteLeft_Children(NodeTypePtr pChild)
	{
		m_Children[CHILD_L] = pChild;
	}

	void OverwriteMiddle_Children(NodeTypePtr pChild)
	{
		m_Children[CHILD_M] = pChild;
	}

	void RemoveValue_R()
	{
		if (m_values[VALUE_R].is_available)
		{
			m_values[VALUE_R].is_available = false;
			//m_value[VALUE_R].value = TValue();  Buna çevirmeye çalýþ yoksa deðer kalýyor yanda sadece is_available deðiþiyor
		}
		else
		{
			assert(false);
		}
	}

	KeyValueInfo m_values[2]; 
	NodeTypePtr m_Children[3];
};


// New class composed of BTreeNode
template <typename TKey, typename TValue>
class BalancedTree
{
public:
	typedef BTreeNode<TKey, TValue> NodeType; //Class Node has consists of BTreeNode's
	typedef NodeType* NodeTypePtr; // Same class but pointer type


private:
	// Root node
	NodeTypePtr &m_pRoot;
	bool m_fOwnTree; // Flag for showing when the Tree should be deleted. For example if the tree is called in a function, at the end of the function tree shouldn't be deleted

public:

	BalancedTree(NodeTypePtr &pRoot, bool fOwnTree)
		: m_pRoot{ pRoot }, m_fOwnTree(fOwnTree)
	{
	}

	~BalancedTree()
	{
		if (m_fOwnTree)
		{
			DestroyTree();
		}
	}

	NodeTypePtr& GetRootRef()
	{
		return m_pRoot;
	}

	NodeTypePtr Detach()
	{
		auto retV = m_pRoot;
		m_pRoot = nullptr;
	}
	
	NodeTypePtr find_my_parent(NodeTypePtr pNode)
	{
		return helper_find_my_parent(pNode, GetRootRef());
	}
	//--------------------Destructor Part ------------------- Start
	void DestroyTree()
	{
		DestroyTreeHelper(m_pRoot);
		m_pRoot = nullptr;
	}
	// Visit all nodes in tree and delete them
	void DestroyTreeHelper(NodeTypePtr pNode)
	{
		if (pNode)
		{
			if (pNode->ChildCount() != 0)
			{
				DestroyTreeHelper(pNode->m_Children[CHILD_L]);
				DestroyTreeHelper(pNode->m_Children[CHILD_M]);
				DestroyTreeHelper(pNode->m_Children[CHILD_R]);
			}
			else
			{
				delete pNode;
				return;
			}

		}
	}
	//--------------------Destructor Part ------------------- Finish


	TValue* get_item_ptr(TKey key)
	{
		NodeTypePtr *pNode = helper_find_value(m_pRoot, key);
		if (!pNode)
		{
			return nullptr;
		}
		return &(pNode->value);
	}

	void insert(TKey key, const TValue &value)
	{
		if (!m_pRoot)
		{
			//Empty tree. Create root and put value there..
			m_pRoot = new NodeType;
			m_pRoot->InsertValue(key, value);
			return;
		}
		NodeTypePtr pParentOfNodeToInsert = 0;
		NodeTypePtr existing_child = 0;
		NodeTypePtr pNodeToInsert = find_node_to_insert(key, pParentOfNodeToInsert);
		insert_into_node(pNodeToInsert, pParentOfNodeToInsert, key, value, existing_child);
	}
	// List the tree in ascending order
	void List(NodeTypePtr pNode)
	{
		if (pNode != nullptr)
		{
			switch (pNode->ChildCount()) // Obtain the information of how many child node has
			{
				case 0: // If there is no child, Look at the values in that node
					if (pNode->ValueCount() == 1)
					{
						cout << pNode->m_values[VALUE_L].key << endl; // If there is just one value, print only left one
					}
					else // If there is two values in the node, print both of them. 
					{
						cout << pNode->m_values[VALUE_L].key << endl;
						cout << pNode->m_values[VALUE_R].key << endl;
					}
					break;
				case 1:
					assert(false);
				case 2: // If there are still child in the node go over them in the order of LNR(left-node-right)
					List(pNode->m_Children[CHILD_L]);
					cout << pNode->m_values[VALUE_L].key << endl;
					List(pNode->m_Children[CHILD_R]);
					break;
				case 3: // A node with three children 
					List(pNode->m_Children[CHILD_L]);
					cout << pNode->m_values[VALUE_L].key << endl;
					List(pNode->m_Children[CHILD_M]);
					cout << pNode->m_values[VALUE_R].key << endl;
					List(pNode->m_Children[CHILD_R]);
					break;
				default:
					break;
			}

		}
	}


	TValue search(TKey key, NodeTypePtr pNode)
	{
		if (pNode == nullptr)
		{
			return 0;
		}
		else
		{

			TKey foundkey = search_in_a_node(pNode, key);

			if (!foundkey)
			{
				while (1)
				{
					if (key < pNode->m_values[VALUE_L].key)
						return search(key, pNode->m_Children[CHILD_L]);
					else if (key > pNode->m_values[VALUE_R].key)
						return search(key, pNode->m_Children[CHILD_R]);
					else
						return search(key, pNode->m_Children[CHILD_M]);
				}

			}
			else
			return search_in_a_node(pNode, key);
		}

	}

	TValue search_in_a_node(NodeTypePtr pNode, TKey key) // Search the key in a node with one or more values.
	{
		if (key == pNode->m_values[VALUE_L].key)
		{
			return pNode->m_values[VALUE_L].value;
		}
		else if (key == pNode->m_values[VALUE_R].key)
		{
			return pNode->m_values[VALUE_R].value;
		}
		else
		{
			return 0;
		}
	}

	void helper_print_node(NodeTypePtr pNode)
	{
		if (!pNode)
		{
			return;
		}
		auto ldPrint = [](const auto &kv)
		{
			if (kv.is_available)
			{
				cout << "[ID:" << kv.key << "-" << "Index:" << kv.value << "] ";
			}
		};
		ldPrint(pNode->m_values[VALUE_L]);
		ldPrint(pNode->m_values[VALUE_R]);
	}

	void print(const char *pszMsg = "")
	{
		cout << pszMsg << std::endl;

		using PrintQueType = std::pair<size_t, NodeTypePtr>;

		std::queue<PrintQueType> printQueue;
		
		PrintQueType element = std::make_pair<size_t, NodeTypePtr>(size_t(0), (NodeTypePtr)m_pRoot);
		printQueue.push(element);

		size_t nPreviouseLevelPrinted = 0;

		while (!printQueue.empty())
		{
			PrintQueType pairCurrent = printQueue.front();
			printQueue.pop();
			
			size_t nCurrentLevel = pairCurrent.first;
			NodeTypePtr nodeNext = pairCurrent.second;

			if (!nodeNext)
			{
				break;
			}
	
			if (nCurrentLevel != nPreviouseLevelPrinted)
			{
				cout << std::endl;
				nPreviouseLevelPrinted = nCurrentLevel;
			}

			helper_print_node(nodeNext);

			if (nodeNext->m_Children[CHILD_L])
				printQueue.push(std::make_pair<size_t, NodeTypePtr>(nCurrentLevel + 1, (NodeTypePtr)nodeNext->m_Children[CHILD_L]));

			if (nodeNext->m_Children[CHILD_M])
				printQueue.push(std::make_pair<size_t, NodeTypePtr>(nCurrentLevel + 1, (NodeTypePtr)nodeNext->m_Children[CHILD_M]));

			if (nodeNext->m_Children[CHILD_R])
				printQueue.push(std::make_pair<size_t, NodeTypePtr>(nCurrentLevel + 1, (NodeTypePtr)nodeNext->m_Children[CHILD_R]));
		}
		cout << std::endl;
	}

private:

	void insert_into_node(NodeTypePtr pNodeToInsert, NodeTypePtr pParentOfNodeToInsert, const TKey key, const TValue value, NodeTypePtr existing_child)
	{
		int ValueCount = pNodeToInsert->ValueCount(); //Number of value in that node

		switch (ValueCount)
		{
			case 0: //A node exist but no value, should not be case
			{
				assert(false);
				return;
			}
			case 1: //A Node has just one value
			{
				pNodeToInsert->InsertValue(key, value);
				break;
			}
			case 2: // A node with two values ( ??? )
			{
				insert_into_full_node(pNodeToInsert, pParentOfNodeToInsert, key, value, existing_child);
			}
		default:
			break;
		}
	}

	// The function in order to find the parent of a node
	NodeTypePtr helper_find_my_parent(NodeTypePtr pNode, NodeTypePtr pRoot)
	{
		if (pRoot == pNode)
		{
			return nullptr;
		}
		// The searching parent is the left side of the root
		else if (pRoot->m_values[VALUE_L].key > pNode->m_values[VALUE_L].key)
		{
			if (pRoot->m_Children[CHILD_L] == pNode)
			{
				return pRoot;
			}
			else
				pRoot = pRoot->m_Children[CHILD_L];
			return helper_find_my_parent(pNode, pRoot);
		}
		//The searching parent is the right side of root
		else if (pRoot->m_values[VALUE_R].key < pNode->m_values[VALUE_L].key)
		{
			if (pRoot->m_Children[CHILD_R] == pNode)
			{
				return pRoot;
			}
			else
				pRoot = pRoot->m_Children[CHILD_R];
			return helper_find_my_parent(pNode, pRoot);
		}
		// The searching parent is the in the middle side of the root
		else
		{
			if (pRoot->m_Children[CHILD_M] == pNode)
			{
				return pRoot;
			}
			else
				pRoot = pRoot->m_Children[CHILD_M];
			return helper_find_my_parent(pNode, pRoot);
		}
	}


	// The new value which will be pushed for balancing
	void push_value_up(NodeTypePtr pNodeTarget, TKey key, TValue valueToPush, NodeTypePtr pNodeExistingChild, NodeTypePtr pInputNewChild) 
	{
		if (nullptr == pNodeTarget)
		{
			//We are trying to push a valueToPush to root's parent, which is naturally null
			NodeTypePtr pNewRoot = new NodeType;
			pNewRoot->InsertValue(key, valueToPush);
			pNewRoot->InsertChildren(pNodeExistingChild, pInputNewChild); //Update children

			m_pRoot = pNewRoot; // Update the root of tree
			return;
		}

		switch (pNodeTarget->ValueCount()) // Obtain the information for how many values in the Target Node if it is 2, there will be again push
		{
			case 0: { assert(false); }
			case 1:
			{
				//First insert the new valueToPush
				pNodeTarget->InsertValue(key, valueToPush);
				pNodeTarget->InsertChildren(pNodeExistingChild, pInputNewChild);
				return;
			}
			case 2:
			{
				//First, find the parent of pNodeTarget
				auto parent_node = find_my_parent(pNodeTarget);

				NodeTypePtr pNewChildAfterSplit_1 = new NodeType;
				NodeTypePtr pSplitNode1 = pNewChildAfterSplit_1;

				NodeTypePtr pNewChildAfterSplit_2 = new NodeType;
				NodeTypePtr pSplitNode2 = pNewChildAfterSplit_2;

				NodeTypePtr a[4];
				if (key > pNodeTarget->m_values[VALUE_R].key)
				{
					a[0] = pNodeTarget->m_Children[CHILD_L];
					a[1] = pNodeTarget->m_Children[CHILD_M];
					a[2] = pNodeExistingChild;
					a[3] = pInputNewChild;
					pSplitNode1->InsertValue(pNodeTarget->m_values[VALUE_L].key, pNodeTarget->m_values[VALUE_L].value);
					pSplitNode2->InsertValue(key, valueToPush);
					key = pNodeTarget->m_values[VALUE_R].key;
					valueToPush = pNodeTarget->m_values[VALUE_R].value;
				}
				else if (key < pNodeTarget->m_values[VALUE_L].key)
				{
					a[2] = pNodeTarget->m_Children[CHILD_M];
					a[3] = pNodeTarget->m_Children[CHILD_R];
					a[0] = pNodeExistingChild;
					a[1] = pInputNewChild;
					pSplitNode1->InsertValue(key, valueToPush);
					pSplitNode2->InsertValue(pNodeTarget->m_values[VALUE_R].key, pNodeTarget->m_values[VALUE_R].value);
					key = pNodeTarget->m_values[VALUE_L].key;
					valueToPush = pNodeTarget->m_values[VALUE_L].value;
				}
				else
				{
					a[0] = pNodeTarget->m_Children[CHILD_L];
					a[3] = pNodeTarget->m_Children[CHILD_R];
					a[2] = pInputNewChild;
					a[1] = pNodeExistingChild;
					pSplitNode1->InsertValue(pNodeTarget->m_values[VALUE_L].key, pNodeTarget->m_values[VALUE_L].value);
					pSplitNode2->InsertValue(pNodeTarget->m_values[VALUE_R].key, pNodeTarget->m_values[VALUE_R].value);
				}

				//SORT a
				/*
				(S1 & S2 also sorted)

					a[0] & a[1]->S1's child'
					a[2] & a[3]->S1's child'

				MOVE R-Value from S1 to S2
					-S2.SetValue (R-Value of S1)
					-Erase R-value of S1

				Swap S1 & S2 so that S1.L.k < S2.L.k
				*/

				//Link pNodeExistingChild to pInputNewChild either pNodeTarget or its split pSplitNode2
				pSplitNode1->InsertChildren(a[0], a[1]);
				pSplitNode2->InsertChildren(a[2], a[3]);

				//Call again push_value in recursive 
				push_value_up(parent_node, key, valueToPush, pSplitNode1, pSplitNode2);

			}
		}
	}

	//-------------------------------------------------------------------------------------------------------------------------
	// Callend only if the (leaf) node to insert into value is alread FULL
	//-------------------------------------------------------------------------------------------------------------------------
	void insert_into_full_node(NodeTypePtr pNodeToInsert, NodeTypePtr pParentOfNodeToInsert, const TKey key, const TValue value, NodeTypePtr existing_child)
	{
		if (key < pNodeToInsert->m_values[VALUE_L].key)
		{
			//Example [100;200] and we try to insert 50

			NodeTypePtr pSplitNode2 = new NodeType;

			//NewNode [200;]
			pSplitNode2->InsertValue(pNodeToInsert->m_values[VALUE_R].key, pNodeToInsert->m_values[VALUE_R].value);
			if (existing_child != nullptr)
			{
				pSplitNode2->InsertChildren(existing_child->m_Children[CHILD_L], existing_child->m_Children[CHILD_R]);
			}


			//Existing Node: [50;]
			//Existing node's R will be killed and L will be replaced by new key-value
			auto kvCopyOfLeftToMoveUp = pNodeToInsert->m_values[VALUE_L];

			pNodeToInsert->RemoveValue_R();

			pNodeToInsert->OverwriteLeft(key, value); //New coming value is lower than left value, so left value has to be changed

			//Value 100 will be pushed up
			push_value_up(pParentOfNodeToInsert, kvCopyOfLeftToMoveUp.key, kvCopyOfLeftToMoveUp.value, pNodeToInsert, pSplitNode2);
		}
		else if (key > pNodeToInsert->m_values[VALUE_R].key)
		{
			//Example [100;200] and we try to insert 250

			NodeTypePtr pSplitNode2 = new NodeType;

			//NewNode [250;]
			pSplitNode2->InsertValue(key, value);
			if (existing_child != nullptr)
			{
				pSplitNode2->InsertChildren(existing_child->m_Children[CHILD_L], existing_child->m_Children[CHILD_R]);
			}

			//Existing node's R will be moved up. Make a copy of it
			auto kvCopyOfLeftToMoveUp = pNodeToInsert->m_values[VALUE_R];

			//void push_value_up(NodeTypePtr pNodeTarget, TKey key, TValue valueToPush, NodeTypePtr pNodeExistingChild, NodeTypePtr pInputNewChild)

			//Keep ony L [100;]
			//NodeTypePtr a[4];

			pNodeToInsert->RemoveValue_R(); // Remove yapmadý bak!!!!!------------ Remove'dan sonra R_child'da sil oda kalýyor yoksa !!!!!

			//Value 200 will be pushed up
			push_value_up(pParentOfNodeToInsert, kvCopyOfLeftToMoveUp.key, kvCopyOfLeftToMoveUp.value, pNodeToInsert, pSplitNode2);
		}
		else
		{
			//Example [100;200] and we try to insert 150

			//New value to the theoretical "middle"

			//Split such that [Lv;Rv] => [Lv; ] [Rv; ]
			NodeTypePtr pSplitNode2  = new NodeType;
			//[200;]
			pSplitNode2->InsertValue(pNodeToInsert->m_values[VALUE_R].key, pNodeToInsert->m_values[VALUE_R].value);
			if (existing_child != nullptr)
			{
				pSplitNode2->InsertChildren(existing_child->m_Children[CHILD_L], existing_child->m_Children[CHILD_R]);
			}
			NodeTypePtr a[4];

			//[100;]
			pNodeToInsert->RemoveValue_R();

			a[0] = pParentOfNodeToInsert->m_Children[CHILD_L];
			a[1] = pSplitNode2;
			a[2] = pParentOfNodeToInsert->m_Children[CHILD_M];
			a[3] = pParentOfNodeToInsert->m_Children[CHILD_R];

			//[175 => move up]
			push_value_up(pParentOfNodeToInsert, key, value, pNodeToInsert, pSplitNode2);
		}
	}

	//-------------------------------------------------------------------------------------------------------------------------
	// Called only if the (leaf) node to insert into value is alread FULL
	//-------------------------------------------------------------------------------------------------------------------------
	void helper_split_node(NodeTypePtr pNodeToSplit, NodeTypePtr &rClone)
	{
		rClone = new NodeType(*pNodeToSplit);
	}

	//----------------------------------------------------------------------------
	// Find a leaf node to insert the new value
	//----------------------------------------------------------------------------
	NodeTypePtr find_node_to_insert(TKey key, NodeTypePtr &pParentOfNodeToInsert)
	{
		if (!m_pRoot)
		{
			assert(false);
			pParentOfNodeToInsert = nullptr;
			return nullptr;
		}
		pParentOfNodeToInsert = nullptr;
		return helper_find_node_to_insert(m_pRoot, key, pParentOfNodeToInsert);
	}

	//The function which finds the node to insert student.
	NodeTypePtr helper_find_node_to_insert(NodeType *pCheck, const TKey &key, NodeTypePtr &pParentOfNodeToInsert)
	{
		assert(pCheck);
		size_t nChildCount = pCheck->ChildCount();
		switch (nChildCount)
		{
			case 0:
			{
				return pCheck;
			}
			case 2:
			{
				pParentOfNodeToInsert = pCheck;
				pCheck = (key <= pCheck->m_values[VALUE_L].key) ? pCheck->m_Children[CHILD_L] : pCheck->m_Children[CHILD_R];
				return helper_find_node_to_insert(pCheck, key, pParentOfNodeToInsert);
			}
			case 3:
			{
				pParentOfNodeToInsert = pCheck;
				if (key <= pCheck->m_values[VALUE_L].key)
				{
					return helper_find_node_to_insert(pCheck->m_Children[CHILD_L], key, pParentOfNodeToInsert);
				}
				if (key > pCheck->m_values[VALUE_R].key)
				{
					return helper_find_node_to_insert(pCheck->m_Children[CHILD_R], key, pParentOfNodeToInsert);
				}
				return helper_find_node_to_insert(pCheck->m_Children[CHILD_M], key, pParentOfNodeToInsert);
			}

			case 1:
			default:
			{
				assert(false);
				return nullptr;
			}
		}
	}

	//-------------------------------------------------------------
	//The function which finds the value of key for searching 
	//-------------------------------------------------------------
	NodeTypePtr helper_find_value(NodeTypePtr pCheck, TKey key)
	{
		assert(pCheck->m_values[VALUE_L].is_available);

		// Look into values in this node
		if (pCheck->m_values[VALUE_L].key == key)
		{
			return pCheck;
		}
		if (pCheck->m_values[VALUE_R].is_available && pCheck->m_values[VALUE_R].key == key)
		{
			return pCheck;
		}

		//Navigate tree
		if (pCheck->m_Children[CHILD_L])
		{
			pCheck = helper_find_value(pCheck->m_Children[CHILD_L], key);
			if (pCheck)
			{
				return pCheck;
			}
		}
		if (pCheck->m_Children[CHILD_M])
		{
			pCheck = helper_find_value(pCheck->m_Children[CHILD_M], key);
			if (pCheck)
			{
				return pCheck;
			}
		}
		if (pCheck->m_Children[CHILD_R])
		{
			pCheck = helper_find_value(pCheck->m_Children[CHILD_R], key);
			if (pCheck)
			{
				return pCheck;
			}
		}
		return NULL;
	}
};


