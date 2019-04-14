#pragma once

#include "Vector2.h"

struct Vector2Vector {
	Vector2 *head;
	Vector2 *tail;
	unsigned int mSize = 0;

	Vector2Vector();


	// Necessary vector functions
	bool empty() const;
	unsigned int size();
	void push_back(Vector2 *e);
	void emplace_back(Vector2 *e);
	Vector2* pop_back();

	void swap(unsigned int i, unsigned int j);

	// [] Operator
	Vector2 operator[](unsigned int index);
};