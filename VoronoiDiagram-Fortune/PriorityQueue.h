#pragma once

#include "Event.h"
#include "EventVector.h"


class PriorityQueue
{
public:
	PriorityQueue()
	{

	}

	// Accessors

	bool isEmpty() const
	{
		return mElements.empty();
	}

	// Operations

	Event *pop()
	{
		swap(0, mElements.size() - 1);
		auto top = mElements.pop_back();
		siftDown(0);
		return top;
	}

	void push(Event *elem)
	{
		elem->index = mElements.size();
		mElements.emplace_back(elem);
		siftUp(mElements.size() - 1);
	}

	void update(unsigned int i)
	{
		int parent = getParent(i);
		if (parent >= 0 && *mElements[parent] < *mElements[i])
			siftUp(i);
		else
			siftDown(i);
	}

	void remove(unsigned int i)
	{
		swap(i, mElements.size() - 1);
		mElements.pop_back();
		if (i < mElements.size())
			update(i);
	}



private:
	eventVector mElements;

	// Accessors

	int getParent(int i) const
	{
		return (i + 1) / 2 - 1;
	}

	unsigned int getLeftChild(unsigned int i) const
	{
		return 2 * (i + 1) - 1;
	}

	unsigned int getRightChild(unsigned int i) const
	{
		return 2 * (i + 1);
	}

	// Operations

	void siftDown(unsigned int i)
	{
		unsigned int left = getLeftChild(i);
		unsigned int right = getRightChild(i);
		unsigned int j = i;
		if (left < mElements.size() && *mElements[j] < *mElements[left])
			j = left;
		if (right < mElements.size() && *mElements[j] < *mElements[right])
			j = right;
		if (j != i)
		{
			swap(i, j);
			siftDown(j);
		}
	}

	void siftUp(unsigned int i)
	{
		int parent = getParent(i);
		if (parent >= 0 && *mElements[parent] < *mElements[i])
		{
			swap(i, parent);
			siftUp(parent);
		}
	}


	inline void swap(unsigned int i, unsigned int j)
	{
		mElements.swap(i,j);
	}
};


