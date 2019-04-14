#pragma once

#include "Event.h"

struct eventVector
{
	Event *head;
	Event *tail;
	unsigned int mSize = 0;

	// Default constructor
	eventVector();
	~eventVector();

	// Necessary vector functions
	bool empty() const;
	unsigned int size();
	void push_back(Event *e);
	void emplace_back(Event *e);
	Event* pop_back();

	void swap(unsigned int i, unsigned int j);
	
	// [] Operator
	Event* operator[](unsigned int index);
};

