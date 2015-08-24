#pragma  once

template <class Node>
class list
{
public:
	list()
	{
		m_head = NULL;
	}

	void PushFront(Node* node)
	{
		node->next = m_head;
		m_head = node;
	}

	Node* PopFront() 
	{
		if (!m_head)
			return NULL;

		Node* ret = m_head;
		m_head = m_head->next;

		return ret;
	}
private:
	Node* m_head;
};

template <class Node>
class delist
{
public:
	delist()
	{
		m_head = NULL;
		m_tail = NULL;
	}

	Node* Front() const
	{
		return m_head;
	}

	Node* GetNext(Node* curNode) const
	{
		if (!curNode)
			return NULL;

		return curNode->next;
	}

	void PushBack(Node* node)
	{
		if (!m_tail) // m_head == NULL
		{
			node->pre = NULL;
			node->next = NULL;
			m_head = node;
			m_tail = node;
		}
		else
		{
			m_tail->next = node;
			node->pre = m_tail;
            node->next = NULL;
			m_tail = node;
		}
	}

	Node* PopFront()
	{
		if (!m_head)
			return NULL;

		Node* node = m_head;
		m_head = m_head->next;

		if (!m_head)
			m_tail = NULL;
		
		return node;
	}

	void Erase(Node* node)
	{
		Node* preNode = node->pre;
		Node* nextNode = node->next;
		if (!preNode && !nextNode) // one node
		{
			m_head = NULL;
		}
		else if (!preNode && nextNode) // head
		{
			nextNode->pre = NULL;
			m_head = nextNode;
		}
		else if (preNode && !nextNode) // tail
		{
			preNode->next = NULL;
		}
		else // preNode && nextNode
		{
			preNode->next = nextNode;
			nextNode->pre = preNode;
		}
	}

    bool IsBack(const Node* node) const
    {
        return m_tail == node;
    }
public:
	Node* m_head;
	Node* m_tail;
};

